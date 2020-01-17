#include "renderer.h"

void
DrawRectangle(orthographic_camera *Camera, vec2 From, vec2 To, color Color)
{
    static shader Shader;
    if (!Shader.ShaderProgram)
    {
        //NOTE(vosure) Relative path doesn't work, magic?
        //NOTE(vosure) free this pointer after you read it, otherwise we get a memory leak
        // NOTE(insolence): FIXED!!!
        Shader = CreateShader("shaders/basic.vert", "shaders/basic.frag");
    }

    unsigned int VAO = 0, VBO = 0;

    float vertices[] = {
            From.X, From.Y,
            From.X, To.Y,
            To.X,   From.Y,
            To.X,   To.Y,
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glUseProgram(Shader.ShaderProgram);
    SetColor("CustomColor", Shader, Color);
    SetMat4("ViewProjection", Shader, Camera->ViewProjection);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
DrawRectangleTextured(orthographic_camera *Camera, vec2 From, vec2 To, texture Texture, color Color = {0.f, 0.f, 0.f})
{
    static shader TexturedShader;
    if (!TexturedShader.ShaderProgram)
    {
        TexturedShader = CreateShader("shaders/texture.vert", "shaders/texture.frag");
    }

    unsigned int TexturedVAO = 0, TexturedVBO = 0;

    // float Vertices[] = {
    //     // Vertices    // TexCoords
    //     From.X, From.Y,  Texture.TexCoords[0], Texture.TexCoords[1],
    //     From.X, To.Y,    Texture.TexCoords[0], Texture.TexCoords[3],
    //     To.X,   From.Y,  Texture.TexCoords[2], Texture.TexCoords[1],
    //     To.X,   To.Y,    Texture.TexCoords[2], Texture.TexCoords[3]
    // };

    // NOTE: These TexCoords are fine cause they are 0 to 1 most of the time anyway
    float Vertices[] = {
        // Vertices      // TexCoords
        From.X, From.Y,  0, 0,
        From.X, To.Y,    0, 1,
        To.X,   From.Y,  1, 0,
        To.X,   To.Y,    1, 1
    };
    glGenVertexArrays(1, &TexturedVAO);
    glGenBuffers(1, &TexturedVBO);

    glBindVertexArray(TexturedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    glBindVertexArray(TexturedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);

    glUseProgram(TexturedShader.ShaderProgram);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);
    SetColor("CustomColor", TexturedShader, Color);
    SetMat4("ViewProjection", TexturedShader, Camera->ViewProjection);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void
RenderScreenTexture(int FBTexture, postprocessing_effects Effects)
{
    static shader FBShader;
    static unsigned int ScreenVAO = 0, ScreenVBO = 0;
    if (!FBShader.ShaderProgram)
    {
        FBShader = CreateShader("shaders/postprocessing.vert", "shaders/postprocessing.frag");

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
    glUseProgram(FBShader.ShaderProgram);
    SetBool("Inversion", FBShader, Effects.Inversion);
    SetBool("Grayscale", FBShader, Effects.Grayscale);
    SetBool("Blur", FBShader, Effects.Blur);
    glBindTexture(GL_TEXTURE_2D, FBTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}