#version 450

layout(location=0)out vec4 fragColor;

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

struct Light 
{
    vec4 position;
    vec4 color;
};

layout(std430, binding = 0) buffer LightsBuffer
{
    Light lights[];
};
  
uniform Material material;
uniform int lightCount;
uniform vec3 viewPos;

in vec2 uv;
in vec3 FragPos;  
in vec3 Normal; 

vec3 computePhong(in Light light)
{
    // ambient
    vec3 ambient = light.color.rgb * material.ambient;
    if (material.hasAmbientTex) ambient *= texture(material.ambientTex, uv).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position.xyz - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diff * material.diffuse;
    if (material.hasDiffuseTex) diffuse *= texture(material.diffuseTex, uv).rgb;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.color.rgb * (spec * material.specular);  
    if (material.hasSpecularTex) specular *= texture(material.specularTex, uv).rgb;

	// emissive
    vec3 emissive = light.color.rgb * material.emissive;
    if (material.hasEmissiveTex) emissive *= texture(material.emissiveTex, uv).rgb;
       
    return ambient + diffuse + specular + emissive;
}

void main()
{
    vec3 color = vec3(0.0, 0.0, 0.0);;

    if (lightCount == 0)
    {
	    // diffuse
        vec3 diffuse = material.diffuse;
		if (material.hasDiffuseTex) diffuse *= texture(material.diffuseTex, uv).rgb;
	    
		color = diffuse;
        
    }
    else
    {
	    for (int i = 0; i < lightCount; i++)
		{
		    Light light = lights[i];
		    color += computePhong(light);
		}
    }

    fragColor = vec4(color, 1.0);
}