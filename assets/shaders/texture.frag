#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec2 FragPos;

uniform sampler2D OurTexture;
uniform vec3 CustomColor;

uniform vec2 LightPos;
uniform vec3 LightColor;

uniform vec3 AmbientLight;

void main()
{
    float Length = length(FragPos - LightPos) * 4;

    vec4 NewLightColor = vec4(LightColor.r/Length, LightColor.g/Length, LightColor.b/Length, 1);
	FragColor = (texture(OurTexture, TexCoord) + vec4(CustomColor, FragColor.a)) * NewLightColor * vec4(AmbientLight, 1);


	float Brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}