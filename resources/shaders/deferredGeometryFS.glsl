#version 450

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

layout(location = 0) out vec3 gBufferPosition;
layout(location = 1) out vec3 gBufferNormal;
layout(location = 2) out vec3 gBufferAmbient;
layout(location = 3) out vec3 gBufferDiffuse;
layout(location = 4) out vec3 gBufferSpecular;
layout(location = 5) out vec3 gBufferEmissive;
layout(location = 6) out vec3 gBufferShininess;
  
uniform Material material;

in vec2 uv;
in vec3 FragPos;  
in vec3 Normal; 

void main()
{
    gBufferPosition = FragPos;

    gBufferNormal = normalize(Normal);

    gBufferAmbient.rgb = material.ambient;
	if (material.hasAmbientTex) gBufferAmbient.rgb *= texture(material.ambientTex, uv).rgb;

    gBufferDiffuse.rgb = material.diffuse;
	if (material.hasDiffuseTex) gBufferDiffuse.rgb *= texture(material.diffuseTex, uv).rgb;

	gBufferSpecular.rgb = material.specular;
	if (material.hasSpecularTex) gBufferSpecular.rgb *= texture(material.specularTex, uv).rgb;

	gBufferEmissive.rgb = material.emissive;
	if (material.hasEmissiveTex) gBufferEmissive.rgb *= texture(material.emissiveTex, uv).rgb;

	gBufferShininess.r = material.shininess;
}