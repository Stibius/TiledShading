#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
   TexCoords = texCoords;
   vec4 worldPos = modelMatrix * vec4(position, 1.0);
   FragPos = worldPos.xyz; 
   mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
   Normal = normalMatrix * normal;
   gl_Position = projectionMatrix * viewMatrix * worldPos;
}