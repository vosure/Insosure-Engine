#version 330 core
out vec4 FragColor; 

uniform vec3 CustomColor;

void main()
{
    FragColor = vec4(CustomColor, 1.0);
}