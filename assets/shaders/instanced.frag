#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;

uniform sampler2D OurTexture;
uniform vec3 CustomColor;

void main()
{
	FragColor = texture(OurTexture, TexCoord) + vec4(CustomColor, FragColor.a);

	float Brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}