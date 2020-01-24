#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 Offset;

uniform mat4 ViewProjection;

out vec2 TexCoord;

void main()
{

	gl_Position = ViewProjection * vec4(aPosition + Offset, 0.0, 1.0);
	TexCoord = aTexCoord;
}