#version 450

layout(location = 0) out vec4 fragColor;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 emissive;
	float shininess;

	sampler2D ambientTex;
	sampler2D diffuseTex;
	sampler2D specularTex;
	sampler2D emissiveTex;

	bool hasAmbientTex;
	bool hasDiffuseTex;
	bool hasSpecularTex;
	bool hasEmissiveTex;
}; 

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
  
uniform Material material;
uniform uint lightCount;
uniform vec3 cameraPos;

in vec3 FragPos;  
in vec3 Normal; 
in vec2 TexCoords;

float computeAttenuation(in PointLight pointLight, in vec3 position)
{
    float dist = distance(pointLight.position.xyz, position);
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

vec3 computePhong(in PointLight pointLight)
{
    float attenuation = computeAttenuation(pointLight, FragPos);
    
	// ambient
    vec3 ambient = pointLight.color.rgb * material.ambient;
    if (material.hasAmbientTex) ambient *= texture(material.ambientTex, TexCoords).rgb;
  	
    // diffuse 
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position.xyz - FragPos);
    float diffuseCoef = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pointLight.color.rgb * diffuseCoef * material.diffuse;
    if (material.hasDiffuseTex) diffuse *= texture(material.diffuseTex, TexCoords).rgb;
    
    // specular
    vec3 cameraDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float specularCoef = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.color.rgb * (specularCoef * material.specular);  
    if (material.hasSpecularTex) specular *= texture(material.specularTex, TexCoords).rgb;

	// emissive
    vec3 emissive = pointLight.color.rgb * material.emissive;
    if (material.hasEmissiveTex) emissive *= texture(material.emissiveTex, TexCoords).rgb;

	ambient  *= attenuation;  
    diffuse  *= attenuation;
    specular *= attenuation; 
       
    return ambient + diffuse + specular + emissive;
}

void main()
{
    vec3 color = vec3(0.0, 0.0, 0.0);;

	if (lightCount == 0)
	{
	    color += material.diffuse;
        if (material.hasDiffuseTex) color *= texture(material.diffuseTex, TexCoords).rgb;
	}
	else
	{
        for (int i = 0; i < lightCount; i++)
	    {
		    PointLight pointLight = pointLights[i];
			
		    color += computePhong(pointLight);
	    }
	}

    fragColor = vec4(color, 1.0);
}