#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Image;

uniform bool Horizontal;
uniform float Weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 TexOffset = 1.0 / textureSize(Image, 0); // NOTE(insolence): Size of a single texel
    vec3 Result = texture(Image, TexCoords).rgb * Weight[0]; // NOTE(insolence): Current fragment's contribution
    if (Horizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            Result += texture(Image, TexCoords + vec2(TexOffset.x * i, 0.0)).rgb * Weight[i];
            Result += texture(Image, TexCoords - vec2(TexOffset.x * i, 0.0)).rgb * Weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            Result += texture(Image, TexCoords + vec2(0.0, TexOffset.y * i)).rgb * Weight[i];
            Result += texture(Image, TexCoords - vec2(0.0, TexOffset.y * i)).rgb * Weight[i];
        }
    }
    FragColor = vec4(Result, 1.0);
}