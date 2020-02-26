#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define MAX_LIGHTS 32

in vec2 TexCoord;
in vec2 FragPos;
in mat3 TBN;

uniform sampler2D OurTexture;
uniform sampler2D NormalMap;

struct dir_light
{
    vec3 Direction;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct point_light
{
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    vec3 CLQ; // NOTE(vosure): Constant, Linear, Quadratic coefs.
};

struct spotlight_light
{
    vec3 Position;
    vec3 Direction;

    float CutOff;
    float OuterCutOff;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    vec3 CLQ;
};

uniform dir_light DirLights[MAX_LIGHTS];
uniform point_light PointLights[MAX_LIGHTS];
uniform spotlight_light SpotLights[MAX_LIGHTS];

uniform vec3 ViewPos;

uniform float Shininess;

// calculates the color when using a directional light.
vec3 CalcDirLight(dir_light Light, vec3 Normal, vec3 ViewDir)
{
    vec3 LightDir = normalize(-Light.Direction);
    // diffuse shading
    float Diff = max(dot(Normal, LightDir), 0.0);
    // specular shading
    vec3 ReflectDir = reflect(-LightDir, Normal);
    float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), Shininess);
    // combine results
    vec3 Ambient = Light.Ambient * vec3(texture(OurTexture, TexCoord));
    vec3 Diffuse = Light.Diffuse * Diff * vec3(texture(OurTexture, TexCoord));
    vec3 Specular = Light.Specular * Spec;
    return (Ambient + Diffuse + Specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(point_light Light, vec3 Normal, vec2 FragPos, vec3 ViewDir)
{

    vec3 LightDir = normalize(vec3(Light.Position.xy - FragPos, 1));
    // diffuse shading
    float Diff = max(dot(Normal, LightDir), 0.0);
    // specular shading
    //vec3 ReflectDir = reflect(-LightDir, Normal);
    //float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), Shininess);
    // attenuation
    float Distance = length(vec3(Light.Position.xy - FragPos, Light.Position.z));
    float Attenuation = 1.0 / (1 + Light.CLQ.y * Distance + Light.CLQ.z * (Distance * Distance));
    // combine results
    vec3 Ambient = Light.Ambient;
    vec3 Diffuse = Light.Diffuse* Diff * vec3(texture(OurTexture, TexCoord));
    //vec3 Specular = Light.Specular * Spec * vec3(texture(OurTexture, TexCoord));
    Ambient *= Attenuation;
    Diffuse *= Attenuation;
    //Specular *= Attenuation;
    return (Ambient + Diffuse);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(spotlight_light Light, vec3 Normal, vec2 FragPos, vec3 ViewDir)
{
    vec3 LightDir = normalize(vec3(Light.Position.xy - FragPos, Light.Position.z));
    // diffuse shading
    float Diff = max(dot(Normal, LightDir), 0.0);
    // specular shading
    vec3 ReflectDir = reflect(-LightDir, Normal);
    float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), Shininess);
    // attenuation
    float Distance = length(vec3(Light.Position.xy - FragPos, Light.Position.z));
    float Attenuation = 1.0 / (Light.CLQ.x + Light.CLQ.y * Distance + Light.CLQ.z * (Distance * Distance));
    // spotlight intensity
    float Theta = dot(LightDir, normalize(-Light.Direction));
    float Epsilon = Light.CutOff - Light.OuterCutOff;
    float Intensity = clamp((Theta - Light.OuterCutOff) / Epsilon, 0.0, 1.0);
    // combine results
    vec3 Ambient = Light.Ambient * vec3(texture(OurTexture, TexCoord));
    vec3 Diffuse = Light.Diffuse * Diff * vec3(texture(OurTexture, TexCoord));
    vec3 Specular = Light.Specular * Spec * vec3(texture(OurTexture, TexCoord));
    Ambient *= Attenuation * Intensity;
    Diffuse *= Attenuation * Intensity;
    Specular *= Attenuation * Intensity;
    return (Ambient + Diffuse + Specular);
}

void main()
{
    vec3 Normal = texture(NormalMap, TexCoord).rgb;
    Normal = normalize(Normal * 2 - 1);
    Normal = normalize(Normal);

    vec3 ViewDir = normalize(ViewPos - vec3(FragPos, 0.0));

    vec3 ResultLightColor;

    // for (int i = 0; i < MAX_LIGHTS; i++)
    // {
    //     ResultLightColor += CalcDirLight(DirLights[i], Normal, ViewDir);
    // }
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        ResultLightColor += CalcPointLight(PointLights[i], Normal, FragPos, ViewDir);
    }
    // for (int i = 0; i < MAX_LIGHTS; i++)
    // {
    //     ResultLightColor += CalcSpotLight(SpotLights[i], Normal, FragPos, ViewDir);
    // }

    FragColor = vec4(ResultLightColor, 1);

	float Brightness = dot(FragColor.rgb, vec3(0.4126, 0.7152, 0.3222));
    if (Brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}