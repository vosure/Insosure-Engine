#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec2 FragPos;
in mat3 TBN;

uniform sampler2D OurTexture;
uniform sampler2D NormalMap;

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

    vec3 Normal = texture(NormalMap, TexCoord).rgb;
    Normal = normalize(Normal * 2.0 - 1.0);
    Normal = normalize(TBN * Normal);
    //Normal.b = -Normal.b;
    Normal.g = -1;
    Normal.r = -1;
    Normal = normalize(Normal);
    //Normal = vec3(0, 0, Normal.b);


    //vec3 Normal = normalize(vec3(0, 0, 1));
    vec3 LightDir = vec3(Light.Position.xy - FragPos, Light.Position.z);
    //LightDir = TBN * LightDir;

    float D = length(LightDir);
    vec3 L = normalize(LightDir);

    vec3 Diffuse = Light.Color * max(dot(Normal, L), 0);
    vec3 Ambient = vec3(0.3, 0.3, 0.3);

    float IntensityFloat = 2;

    vec3 Falloff = vec3(1, 0, 0);
    float Attenuation = 1/(Falloff.x + Falloff.y*D + Falloff.z*D*D);
    vec3 Intensity = (Ambient + Diffuse) * Attenuation * IntensityFloat;


    //vec3 Intensity = Ambient + Diffuse;
    FragColor = Color * vec4(Intensity, 1);

    // float Distance = distance(Light.Position, vec3(FragPos, 0.0));
    // float LightStrength = 0.0;

    // if (Distance <= Light.Radius)
    //     LightStrength =  (1.0 - abs(Distance / Light.Radius)) * Light.Radius;

    //FragColor = Color * ((vec4(Light.Color, 1.0) * LightStrength * Light.Intensity) + vec4(0.3, 0.3, 0.3, 1.0));

	float Brightness = dot(FragColor.rgb, vec3(0.4126, 0.7152, 0.3222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}