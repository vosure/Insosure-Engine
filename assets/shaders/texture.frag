#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D OurTexture;
uniform vec3 CustomColor;

void main()
{
	vec4 Color = texture(OurTexture, TexCoord);
	FragColor = Color + vec4(CustomColor, Color.a);
}