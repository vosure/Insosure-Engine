#version 330 core
layout (location = 0) in vec4 Vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;

uniform mat4 ViewProjection;

void main()
{
    gl_Position = ViewProjection * vec4(Vertex.xy, 0.0, 1.0);
    TexCoords = Vertex.zw;
}