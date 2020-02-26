#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

const int MAX_POINT_LIGHTS = 64;

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

uniform point_light Lights[MAX_POINT_LIGHTS];

vec3 CalcPointLight(point_light PointLight, vec2 FragPos, vec3 Normal)
{
    vec3 LightDir = vec3(PointLight.Position.xy - FragPos, PointLight.Position.z);

    float D = length(LightDir);
    vec3 L = normalize(LightDir);

    vec3 Diffuse = PointLight.Color * max(dot(Normal, L), 0.0);

    vec3 Falloff = vec3(1, 1, PointLight.Radius); // TODO(insolence): Fine-tune these
    float Attenuation = 1/(Falloff.x + Falloff.y*D + Falloff.z*D*D);

    Diffuse *=  Attenuation * PointLight.Intensity;

    return Diffuse;
}

void main()
{
	vec4 Color = texture(OurTexture, TexCoord);
    if (Color.a < 0.1)
        discard;
    vec3 Normal = texture(NormalMap, TexCoord).rgb;
    Normal = normalize(Normal * 2 - 1);
    Normal = normalize(TBN * Normal);

    vec3 Ambient = vec3(0.25, 0.25, 0.25);

    vec3 ResultLightColor = Ambient;
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        if (Lights[i].Intensity > 0)
            ResultLightColor += CalcPointLight(Lights[i], FragPos, Normal);
    }

    FragColor = Color * vec4(ResultLightColor, 1);

	float Brightness = dot(FragColor.rgb, vec3(0.4126, 0.7152, 0.3222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}