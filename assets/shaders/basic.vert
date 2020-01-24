#version 330 core
layout (location = 0) in vec2 aPosition;

uniform mat4 Transform;
uniform mat4 ViewProjection;

void main()
{
    gl_Position = ViewProjection * Transform *  vec4(aPosition, 0.0, 1.0);
}