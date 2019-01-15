#version 450

out vec2 TexCoords;

void main()
{
    if (gl_VertexID == 0)
    {
        gl_Position = vec4(-1, -1, 0, 1);
		TexCoords = vec2(0, 0);
    }
    else if (gl_VertexID == 1)
    {
        gl_Position = vec4(-1, 1, 0, 1);
		TexCoords = vec2(0, 1);
    }
    else if (gl_VertexID == 2)
    {
        gl_Position = vec4(1, -1, 0, 1);
		TexCoords = vec2(1, 0);
    }
	else
	{
	    gl_Position = vec4(1, 1, 0, 1);
		TexCoords = vec2(1, 1);
	}
} 