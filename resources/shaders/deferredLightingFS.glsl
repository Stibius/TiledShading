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
  
uniform int lightCount;
uniform vec3 viewPos;
uniform vec2 screenSize;

uniform sampler2D gBufferPosition;
uniform sampler2D gBufferNormal;
uniform sampler2D gBufferAmbient;
uniform sampler2D gBufferDiffuse;
uniform sampler2D gBufferSpecular;
uniform sampler2D gBufferEmissive;
uniform sampler2D gBufferShininess;

vec3 computePhong()
{
    vec2 uv = gl_FragCoord.xy / screenSize;
    float dist = distance(pointLight.position.xyz, texture(gBufferPosition, uv).rgb);

    vec3 FragPos = texture(gBufferPosition, uv).rgb;
    vec3 Normal = texture(gBufferNormal, uv).rgb;
	vec3 Ambient = texture(gBufferAmbient, uv).rgb;
    vec3 Diffuse = texture(gBufferDiffuse, uv).rgb;
	vec3 Specular = texture(gBufferSpecular, uv).rgb;
	vec3 Emissive = texture(gBufferEmissive, uv).rgb;
    float Shininess = texture(gBufferShininess, uv).r;

	//attenuation: 1 / (fc + fl*distance + fq*distance*distance)
	float cutoff = 0.01;                               //minimum light intensity
	float cutoffDist = sqrt((1 / cutoff) - 1);         //distance to the cutoff
	float radiusCoef = cutoffDist / pointLight.radius; //coef to get the radius we want
	float dist2 = dist * radiusCoef;
	float attenuation = 1 / (pointLight.constantAttenuationFactor + pointLight.linearAttenuationFactor*dist2 + pointLight.quadraticAttenuationFactor*dist2*dist2);
    //Bias by -cutoff and then scale by 1/(1-cutoff) so intensity is zero at radius distance and maximum intensity is unchanged
	attenuation = (attenuation - cutoff) / (1 - cutoff);

    // ambient
    vec3 ambient = pointLight.color.rgb * Ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position.xyz - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.color.rgb * diff * Diffuse;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular = pointLight.color.rgb * (spec * Specular);  

	// emissive
    vec3 emissive = pointLight.color.rgb * Emissive;

	ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation; 
       
    return ambient + diffuse + specular + emissive;
}

void main()
{
	vec3 color = computePhong();

    fragColor = vec4(color, 1.0);
}