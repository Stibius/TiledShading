#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoords;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out vec3 FragPos;
out vec3 Normal;

void main()
{
   uv = texcoords;
   FragPos = pos;
   Normal = normal; 
   gl_Position = projection * view * vec4(pos, 1.0);
}