#include "../utils/file_utils.h"
#include "renderer.h"
#include "texture.h"
#include "shader.h"

void 
DrawRectangle(float FromX, float FromY, float ToX, float ToY, color Color)
{
    static unsigned int VAO = 0, VBO = 0, ShaderProgram = 0;
    if (!VAO)
    {
        float vertices[] = {
            FromX, FromY,
            FromX, ToY,
            ToX,   FromY,
            ToX,   ToY, 
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        //NOTE(vosure) Relative path doesn't work, magic?
        //NOTE(vosure) free this pointer after you read it, otherwise we get a memory leak
        const char* VertexSrc = (const char *)ReadFile("shaders/basic.vert");
        const char* FragSrc   = (const char *)ReadFile("shaders/basic.frag");
        ShaderProgram = CreateShaderProgram(VertexSrc, FragSrc);
        free((void *)VertexSrc);
        free((void *)FragSrc);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glUseProgram(ShaderProgram);
    SetColor("CustomColor", ShaderProgram, Color);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void 
DrawRectangleTextured(float FromX, float FromY, float ToX, float ToY, texture Texture, color Color = {0.f, 0.f, 0.f})
{
    static unsigned int TexturedVAO = 0, TexturedVBO = 0, TexturedShaderProgram = 0;
    if (!TexturedVAO)
    {
        // TODO(Insolence): too hard to understand, maybe change with subdata
        float Vertices[] = {
            // Vertices    // TexCoords
            FromX, FromY,  Texture.TexCoords[0], Texture.TexCoords[1],
            FromX, ToY,    Texture.TexCoords[0], Texture.TexCoords[3],
            ToX,   FromY,  Texture.TexCoords[2], Texture.TexCoords[1],
            ToX,   ToY,    Texture.TexCoords[2], Texture.TexCoords[3],
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

        const char *VertexSrc = (const char *)ReadFile("shaders/texture.vert");
        const char *FragSrc   = (const char *)ReadFile("shaders/texture.frag");
        TexturedShaderProgram = CreateShaderProgram(VertexSrc, FragSrc);
        free((void *)VertexSrc);
        free((void *)FragSrc);
    }

    glBindVertexArray(TexturedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);

    glUseProgram(TexturedShaderProgram);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);
    SetColor("CustomColor", TexturedShaderProgram, Color);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}