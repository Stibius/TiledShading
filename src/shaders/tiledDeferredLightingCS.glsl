#version 450

const uint MAX_LIGHTS_PER_TILE = 256;
const uint WORK_GROUP_SIZE = 16;

layout (local_size_x = WORK_GROUP_SIZE, local_size_y = WORK_GROUP_SIZE) in;

struct PointLight 
{
    vec4 position;
    vec4 color;
	float radius;
	float constantAttenuationFactor;
	float linearAttenuationFactor;
	float quadraticAttenuationFactor;
};

layout(std430, binding = 0) buffer LightsBuffer
{
    PointLight pointLights[];
};

uniform float fov;
uniform float near;
uniform float far;
uniform uint lightCount;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraPos;
uniform uvec2 screenSize;
uniform bool showTiles;

uniform sampler2D gBufferPosition;
uniform sampler2D gBufferNormal;
uniform sampler2D gBufferAmbient;
uniform sampler2D gBufferDiffuse;
uniform sampler2D gBufferSpecular;
uniform sampler2D gBufferEmissive;
uniform sampler2D gBufferShininess;

layout(rgba32f) uniform image2D outputTex;

shared uint lightIdsInTile[MAX_LIGHTS_PER_TILE];
shared uint lightCountInTile;
shared uint minDepth;
shared uint maxDepth;

float computeAttenuation(in PointLight pointLight, in vec3 position)
{
    float dist = distance(pointLight.position.xyz, position);

	if (dist > pointLight.radius)
	{
		return 0.0;
	}

	float fl = pointLight.linearAttenuationFactor;
	float fc = pointLight.constantAttenuationFactor;
	float fq = pointLight.quadraticAttenuationFactor;
	//attenuation: 1 / (fc + fl * distance + fq * distance * distance)
	float minAtt = 0.01; //minimum attenuation
	//(fq * minAttDist * minAttDist) + (fl * minAttDist) + (fc - (1 / minAtt)) = 0
	float discriminant = fl * fl - 4 * fq * (fc - (1 / minAtt));
	float minAttDist = (-fl + sqrt(discriminant)) / (2 * fq); //distance to the cutoff
	float radiusCoef = minAttDist / pointLight.radius;        //coef to get the radius we want
	float dist2 = dist * radiusCoef;
	float attenuation = 1 / (fc + fl * dist2 + fq * dist2 * dist2);
    //Bias by -minAtt and then scale by 1/(1-minAtt) so intensity is zero at radius distance and maximum intensity is unchanged
	attenuation = (attenuation - minAtt) / (1 - minAtt);

	return attenuation;
}

vec3 computePhong(PointLight pointLight, 
                  vec3 fragPos, 
				  vec3 normalTex, 
				  vec3 ambientTex, 
				  vec3 diffuseTex, 
				  vec3 specularTex, 
				  vec3 emissiveTex, 
				  float shininessTex)
{
    float attenuation = computeAttenuation(pointLight, fragPos);
    
    // ambient
    vec3 ambient = pointLight.color.rgb * ambientTex;
  	
    // diffuse 
    vec3 lightDir = normalize(pointLight.position.xyz - fragPos);
    float diffuseCoef = max(dot(normalTex, lightDir), 0.0);
    vec3 diffuse = pointLight.color.rgb * diffuseCoef * diffuseTex;
    
    // specular
    vec3 cameraDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normalTex);  
    float specularCoef = pow(max(dot(cameraDir, reflectDir), 0.0), shininessTex);
    vec3 specular = pointLight.color.rgb * (specularCoef * specularTex);  

	// emissive
    vec3 emissive = pointLight.color.rgb * emissiveTex;

	ambient  *= attenuation;  
    diffuse  *= attenuation;
    specular *= attenuation; 
       
    return ambient + diffuse + specular + emissive;
}

void main()
{
	vec2 texCoords = gl_GlobalInvocationID.xy / (screenSize - vec2(1, 1));
	vec3 fragPos = texture(gBufferPosition, texCoords).rgb;
	vec3 fragPosView = vec3(viewMatrix * vec4(fragPos, 1.0));
	float depthf = (abs(fragPosView.z) - abs(near)) / abs(far - near);
	uint depth = uint(depthf * 0xFFFFFFFF);
	if (gl_LocalInvocationID.x == 0 && gl_LocalInvocationID.y == 0)
	{
	    lightCountInTile = 0;
		minDepth = 0xFFFFFFFF;
        maxDepth = 0;
	}

	barrier();

    atomicMin(minDepth, depth);
    atomicMax(maxDepth, depth);

	barrier();

	float minDepthf = minDepth / float(0xFFFFFFFF);
    float maxDepthf = maxDepth / float(0xFFFFFFFF);
	
    float aspect = screenSize.x / float(screenSize.y);
	float viewportHalfHeight = tan(fov / 2.0);
	float viewportHalfWidth = viewportHalfHeight * aspect;
	vec2 tileScale = gl_WorkGroupSize.xy / vec2(screenSize);
	vec2 tileSize = vec2(viewportHalfWidth * 2 * tileScale.x, viewportHalfHeight * 2 * tileScale.y);
	vec3 viewportBottomLeftCorner = vec3(-viewportHalfWidth, -viewportHalfHeight, -1.0);
	vec3 tileBottomLeftCorner = vec3(
	    viewportBottomLeftCorner.x + tileSize.x * gl_WorkGroupID.x,
		viewportBottomLeftCorner.y + tileSize.y * gl_WorkGroupID.y,
	    -1.0);
	vec3 tileBottomRightCorner = tileBottomLeftCorner + vec3(tileSize.x, 0.0, 0.0);
	vec3 tileTopLeftCorner = tileBottomLeftCorner + vec3(0.0, tileSize.y, 0.0);
	vec3 tileTopRightCorner = tileTopLeftCorner + vec3(tileSize.x, 0.0, 0.0);
			
	vec3 frustumNormals[4];
	//top
	frustumNormals[0] = normalize(cross(tileTopLeftCorner, tileTopRightCorner));
	//right
	frustumNormals[1] = normalize(cross(tileTopRightCorner, tileBottomRightCorner));
	//bottom
	frustumNormals[2] = normalize(cross(tileBottomRightCorner, tileBottomLeftCorner));
	//left
	frustumNormals[3] = normalize(cross(tileBottomLeftCorner, tileTopLeftCorner));
	
	uint localThreadCount = gl_WorkGroupSize.x * gl_WorkGroupSize.y;
	uint iterationCount = (lightCount / localThreadCount) + 1;	

	for (int i = 0; i < iterationCount; i++)
	{
	    uint lightId = (i * localThreadCount) + gl_LocalInvocationIndex;

	    if (lightId < lightCount && lightCountInTile < MAX_LIGHTS_PER_TILE)
		{
		    PointLight pointLight = pointLights[lightId];
		    vec3 lightPosView = vec3(viewMatrix * pointLight.position);
			float lightDepthf = (abs(lightPosView.z) - abs(near)) / abs(far - near);

			if (lightDepthf - pointLight.radius < maxDepthf && lightDepthf + pointLight.radius > minDepthf)
            {
		        bool inFrustum = true;
		        for (uint j = 0; j < 4 && inFrustum; j++)
		        {
			        float distance = dot(vec3(frustumNormals[j]), lightPosView);
		            inFrustum = (-pointLight.radius <= distance);
		        }

		        if (inFrustum)
		        {
			        uint index = atomicAdd(lightCountInTile, 1);
					if (index < MAX_LIGHTS_PER_TILE)
					{
					    lightIdsInTile[index] = lightId;
					}
		        }
			}
		}
	}
	
	barrier();

	vec3 color = vec3(0.0, 0.0, 0.0);

	if (lightCountInTile != 0)
	{
        vec3 normal = normalize(texture(gBufferNormal, texCoords).rgb);
	    vec3 ambient = texture(gBufferAmbient, texCoords).rgb;
        vec3 diffuse = texture(gBufferDiffuse, texCoords).rgb;
	    vec3 specular = texture(gBufferSpecular, texCoords).rgb;
	    vec3 emissive = texture(gBufferEmissive, texCoords).rgb;
        float shininess = texture(gBufferShininess, texCoords).r;
	
	    for (int i = 0; i < lightCountInTile; i++)
	    {
		    PointLight pointLight = pointLights[lightIdsInTile[i]];
		    color += computePhong(pointLight, fragPos, normal, ambient, diffuse, specular, emissive, shininess);
	    }
	}

	if (showTiles)
	{
	    imageStore(outputTex, ivec2(gl_GlobalInvocationID.xy), vec4(lightCountInTile / float(MAX_LIGHTS_PER_TILE), lightCountInTile / float(MAX_LIGHTS_PER_TILE), lightCountInTile / float(MAX_LIGHTS_PER_TILE), 1.0f));
	}
	else
	{
		imageStore(outputTex, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.0));
	}
}