#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D ScreenTexture;

uniform bool Inversion;
uniform bool Grayscale;
uniform bool Blur;

const float Offset = 1.0 / 300.0;  

void main()
{ 
    if (Inversion)
    {
        FragColor = vec4(vec3(1.0 - texture(ScreenTexture, TexCoords)), 1.0);
    }
    else if (Grayscale)
    {
        FragColor = texture(ScreenTexture, TexCoords);
        float Average = 0.2126 * FragColor.r + 0.7 * FragColor.g + 0.07 * FragColor.b;
        FragColor = vec4(Average, Average, Average, 1.0);
    }
    else if (Blur)
    {
        // Must be optimized, probably by calculating everything outside the shader
        vec2 Offsets[9] = vec2[] (
            vec2(-Offset,  Offset), // top-left
            vec2( 0.0f,    Offset), // top-center
            vec2( Offset,  Offset), // top-right
            vec2(-Offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( Offset,  0.0f),   // center-right
            vec2(-Offset, -Offset), // bottom-left
            vec2( 0.0f,   -Offset), // bottom-center
            vec2( Offset, -Offset)  // bottom-right    
        );

        float Kernel[9] = float[] (
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16  
        );

        vec3 SampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            SampleTex[i] = vec3(texture(ScreenTexture, TexCoords.st + Offsets[i]));
        }
        vec3 Col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            Col += SampleTex[i] * Kernel[i];

        FragColor = vec4(Col, 1.0);
    }
    else
    {
        FragColor = texture(ScreenTexture, TexCoords);
    }

}