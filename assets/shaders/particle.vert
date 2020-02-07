#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 ViewProjection;
uniform vec2 Offset;
uniform vec4 Color;
uniform float Size;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = ViewProjection * vec4((aPosition + Offset) * Size, 0.0, 1.0);
}