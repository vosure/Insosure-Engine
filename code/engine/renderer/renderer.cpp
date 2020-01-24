#include "renderer.h"

void
DrawRectangle(orthographic_camera *Camera, mat4 Transform, color Color)
{
    static shader Shader;
    static unsigned int VAO = 0, VBO = 0;
    if (!Shader.ShaderProgram)
    {
        Shader = CreateShader("shaders/basic.vert", "shaders/basic.frag");

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        float Vertices[] = {
            -0.5f,  -0.5f,
            -0.5f,   0.5f,
             0.5f,  -0.5f,
             0.5f,   0.5f,
        };
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    }

    glBindVertexArray(VAO);
    glUseProgram(Shader.ShaderProgram);
    SetColor("CustomColor", Shader, Color);
    SetMat4("ViewProjection", Shader, Camera->ViewProjection);
    SetMat4("Transform", Shader, Transform);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void
DrawRectangleTextured(orthographic_camera *Camera, mat4 Transform, texture Texture, color Color = {0.f, 0.f, 0.f})
{
    static shader TexturedShader;
    static unsigned int TexturedVAO = 0, TexturedVBO = 0;
    if (!TexturedShader.ShaderProgram)
    {
        TexturedShader = CreateShader("shaders/texture.vert", "shaders/texture.frag");

        glGenVertexArrays(1, &TexturedVAO);
        glBindVertexArray(TexturedVAO);

        float Vertices[] = {
            // Vertices        TexCoords
            -0.5f,  -0.5f,     0, 0,
            -0.5f,   0.5f,     0, 1,
             0.5f,  -0.5f,     1, 0,
             0.5f,   0.5f,     1, 1
        };
        glGenBuffers(1, &TexturedVBO);
        glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);
    }

    glUseProgram(TexturedShader.ShaderProgram);
    glBindVertexArray(TexturedVAO);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);
    SetColor("CustomColor", TexturedShader, Color);
    SetMat4("ViewProjection", TexturedShader, Camera->ViewProjection);
    SetMat4("Transform", TexturedShader, Transform);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void
RenderScreenTexture()
{
    static unsigned int ScreenVAO = 0, ScreenVBO = 0;
    if (!ScreenVAO)
    {
        float ScreenVertices[] = {
             // positions  // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &ScreenVAO);
        glGenBuffers(1, &ScreenVBO);

        glBindVertexArray(ScreenVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ScreenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenVertices), &ScreenVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);
    }

    glBindVertexArray(ScreenVAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void
PostprocessScreenTexture(int FBTexture, postprocessing_effects Effects)
{
    static shader FBShader;
    if (!FBShader.ShaderProgram)
    {
        FBShader = CreateShader("shaders/postprocessing.vert", "shaders/postprocessing.frag");
    }
    glUseProgram(FBShader.ShaderProgram);
    SetBool("Inversion", FBShader, Effects.Inversion);
    SetBool("Grayscale", FBShader, Effects.Grayscale);
    SetBool("Blur", FBShader, Effects.Blur);
    glBindTexture(GL_TEXTURE_2D, FBTexture);

    RenderScreenTexture();
}

void
ApplyHDR(int ScreenTexture, int BloomTexture, float Exposure)

{
    static shader HDRShader;
    if (!HDRShader.ShaderProgram)
    {
        HDRShader = CreateShader("shaders/hdr.vert", "shaders/hdr.frag");
        glUseProgram(HDRShader.ShaderProgram);
        SetInt("Scene", HDRShader, 1);
        SetInt("BloomBlur", HDRShader, 0);

    }
    glUseProgram(HDRShader.ShaderProgram);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ScreenTexture);
    SetInt("Scene", HDRShader, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, BloomTexture);
    SetInt("BloomBlur", HDRShader, 0);
    SetFloat("Exposure", HDRShader, Exposure);

    RenderScreenTexture();
}

///////////////////////////////////////
// NOTE(insolence): Instanced rendering
///////////////////////////////////////

void
InstancedDrawRectangleTextured(orthographic_camera *Camera, mat4 *Transforms, int Amount, texture Texture, color Color = {0.f, 0.f, 0.f})
{

    static shader InstancedShader;
    if (!InstancedShader.ShaderProgram)
    {
        InstancedShader = CreateShader("shaders/instanced.vert", "shaders/instanced.frag");
    }

    unsigned int InstancedVAO = 0, InstancedVBO = 0;

    glGenVertexArrays(1, &InstancedVAO);
    glBindVertexArray(InstancedVAO);

    float Vertices[] = {
        // Vertices        TexCoords
        -0.5f,  -0.5f,     0, 0,
        -0.5f,   0.5f,     0, 1,
         0.5f,  -0.5f,     1, 0,
         0.5f,   0.5f,     1, 1
    };
    glGenBuffers(1, &InstancedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, InstancedVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    unsigned int TransformsVBO = 0;
    glGenBuffers(1, &TransformsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, TransformsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * Amount, &Transforms[0], GL_STATIC_DRAW);

    int vec4Size = sizeof(vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glUseProgram(InstancedShader.ShaderProgram);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);
    SetColor("CustomColor", InstancedShader, Color);
    SetMat4("ViewProjection", InstancedShader, Camera->ViewProjection);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Amount);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
