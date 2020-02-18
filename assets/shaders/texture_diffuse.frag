#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec2 FragPos;

uniform sampler2D OurTexture;

uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 AmbientLight;

float LightRadius = 4.0; // 0 - 4

void main()
{
	vec4 Color = texture(OurTexture, TexCoord);
    if (Color.a < 0.1)
        discard;

    float Distance = distance(LightPos.xy, FragPos);
    float LightStrength = 0.0;

    if (Distance <= LightRadius)
        LightStrength =  (1.0 - abs(Distance / LightRadius)) * 0.5;

    FragColor = Color * (vec4(LightColor, 1.0) * LightStrength * 3.5);

	float Brightness = dot(FragColor.rgb, vec3(0.4126, 0.7152, 0.3222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}