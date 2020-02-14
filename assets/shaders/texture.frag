#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec2 FragPos;
in vec3 Normal;

uniform sampler2D OurTexture;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 AmbientLight;

void main()
{
    vec3 LightDir = normalize(LightPos - vec3(FragPos, 0));
    float Diff = max(dot(normalize(Normal), LightDir), 0);
    vec3 Diffuse = Diff * LightColor;

	FragColor = texture(OurTexture, TexCoord) * vec4(Diffuse + AmbientLight, 1.0);

    //float Length = 1.0 / (length(FragPos - LightPos) * length(FragPos - LightPos) * 1.5f);
    //vec3 NewLightColor = LightColor * Length;
	//FragColor = texture(OurTexture, TexCoord) * (NewLightColor + vec4(AmbientLight, 1.0));

	float Brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}