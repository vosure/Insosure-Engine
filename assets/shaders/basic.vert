#version 330 core
layout (location = 0) in vec2 Position;

uniform mat4 ViewProjection;

void main()
{
    gl_Position = ViewProjection * vec4(Position, 0.0, 1.0);
    //gl_Position =  vec4(Position, 0.0, 1.0);
}