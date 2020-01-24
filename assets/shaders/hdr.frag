#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Scene;
uniform sampler2D BloomBlur;
uniform float Exposure;

void main()
{
    vec3 HdrColor = texture(Scene, TexCoords).rgb;
    vec3 BloomColor = texture(BloomBlur, TexCoords).rgb;
    HdrColor += BloomColor;

    // Exposure tone mapping
    vec3 Result = vec3(1.0) - exp(-HdrColor * Exposure);
    // Gamma correct
    const float Gamma = 1.7f; // FIXME(insolence): Fix this to 2.2f or fine-tune
    Result = pow(Result, vec3(1.0 / Gamma));

    FragColor = vec4(Result, 1.0);
}