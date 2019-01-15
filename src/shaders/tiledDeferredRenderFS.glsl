#version 450

layout(location = 0) out vec4 fragColor;

in vec2 TexCoords;

uniform uvec2 screenSize;

layout(rgba32f) uniform image2D tex;

void main()
{
	fragColor = imageLoad(tex, ivec2(TexCoords * screenSize));
}