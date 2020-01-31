#version 330 core
out vec4 Color;

in vec2 TexCoords;

uniform sampler2D Text;
uniform vec3 TextColor;

void main()
{    
    vec4 Sampled = vec4(1.0, 1.0, 1.0, texture(Text, TexCoords).r);
    Color = vec4(TextColor, 1.0) * Sampled;
}  