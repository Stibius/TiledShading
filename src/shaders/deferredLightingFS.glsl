#version 450

layout(location = 0) out vec4 fragColor;

struct PointLight 
{
    vec4 position;
    vec4 color;
	float radius;
	float constantAttenuationFactor;
	float linearAttenuationFactor;
	float quadraticAttenuationFactor;
};

uniform PointLight pointLight;
  
uniform vec3 cameraPos;
uniform uvec2 screenSize;

uniform sampler2D gBufferPosition;
uniform sampler2D gBufferNormal;
uniform sampler2D gBufferAmbient;
uniform sampler2D gBufferDiffuse;
uniform sampler2D gBufferSpecular;
uniform sampler2D gBufferEmissive;
uniform sampler2D gBufferShininess;

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

vec3 computePhong()
{
    vec2 texCoords = gl_FragCoord.xy / screenSize;

    vec3 fragPos = texture(gBufferPosition, texCoords).rgb;
    vec3 normalTex = texture(gBufferNormal, texCoords).rgb;
	vec3 ambientTex = texture(gBufferAmbient, texCoords).rgb;
    vec3 diffuseTex = texture(gBufferDiffuse, texCoords).rgb;
	vec3 specularTex = texture(gBufferSpecular, texCoords).rgb;
	vec3 emissiveTex = texture(gBufferEmissive, texCoords).rgb;
    float shininessTex = texture(gBufferShininess, texCoords).r;

	float attenuation = computeAttenuation(pointLight, texture(gBufferPosition, texCoords).rgb);

    // ambient
    vec3 ambient = pointLight.color.rgb * ambientTex;
  	
    // diffuse 
    vec3 normal = normalize(normalTex);
    vec3 lightDir = normalize(pointLight.position.xyz - fragPos);
    float diffuseCoef = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pointLight.color.rgb * diffuseCoef * diffuseTex;
    
    // specular
    vec3 cameraDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float specularCoef = pow(max(dot(cameraDir, reflectDir), 0.0), shininessTex);
    vec3 specular = pointLight.color.rgb * specularCoef * specularTex;  

	// emissive
    vec3 emissive = pointLight.color.rgb * emissiveTex;

	ambient  *= attenuation;  
    diffuse  *= attenuation;
    specular *= attenuation; 
       
    return ambient + diffuse + specular + emissive;
}

void main()
{
	vec3 color = computePhong();

    fragColor = vec4(color, 1.0);
}