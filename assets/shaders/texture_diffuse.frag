#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec2 FragPos;

uniform sampler2D OurTexture;

struct point_light
{
    vec3 Position;
    vec3 Color;
    float Radius;
    float Intensity;
};

uniform point_light Light;

void main()
{
	vec4 Color = texture(OurTexture, TexCoord);
    if (Color.a < 0.1)
        discard;

    float Distance = distance(Light.Position.xy, FragPos);
    float LightStrength = 0.0;

    if (Distance <= Light.Radius)
        LightStrength =  (1.0 - abs(Distance / Light.Radius)) * Light.Radius;

    FragColor = Color * (vec4(Light.Color, 1.0) * LightStrength * Light.Intensity);

	float Brightness = dot(FragColor.rgb, vec3(0.4126, 0.7152, 0.3222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}