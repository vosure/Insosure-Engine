#include "renderer.h"

void
DrawRectangle(orthographic_camera *Camera, mat4 Transform, color Color)
{
    unsigned int VAO = 0, VBO = 0;
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

    glBindVertexArray(VAO);
    glUseProgram(Shader.ShaderProgram);
    SetColor("CustomColor", Shader, Color);
    SetMat4("ViewProjection", Shader, Camera->ViewProjection);
    SetMat4("Transform", Shader, Transform);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void
DrawTriangle(orthographic_camera *Camera, mat4 Transform, color Color)
{
    unsigned int VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    float Vertices[] = {
        -1.0, 0.0,
         0.0, 1.0,
         1.0, 0.0
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(VAO);
    glUseProgram(Shader.ShaderProgram);
    SetColor("CustomColor", Shader, Color);
    SetMat4("ViewProjection", Shader, Camera->ViewProjection);
    SetMat4("Transform", Shader, Transform);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void
DrawRectangleTextured(orthographic_camera *Camera, mat4 Transform, uint Texture, color Color = {0.f, 0.f, 0.f})
{
    unsigned int TexturedVAO = 0, TexturedVBO = 0;
    glGenVertexArrays(1, &TexturedVAO);
    glBindVertexArray(TexturedVAO);

    float Vertices[] = {
      // Vertices      TexCoords
        -0.5f, -0.5f,  0, 0,
        -0.5f,  0.5f,  0, 1,
         0.5f, -0.5f,  1, 0,
         0.5f,  0.5f,  1, 1
    };
    glGenBuffers(1, &TexturedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    glUseProgram(TexturedShader.ShaderProgram);
    glBindVertexArray(TexturedVAO);
    glBindTexture(GL_TEXTURE_2D, Texture);
    SetColor("CustomColor", TexturedShader, Color);
    SetMat4("ViewProjection", TexturedShader, Camera->ViewProjection);
    SetMat4("Transform", TexturedShader, Transform);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &TexturedVAO);
    glDeleteBuffers(1, &TexturedVBO);
}

// FIXME(insolence): Make this instanced rendering
void
DrawParticles(orthographic_camera *Camera, std::vector<particle> &Particles, uint Texture)
{
    unsigned int ParticleVAO = 0, ParticleVBO = 0;
    glGenVertexArrays(1, &ParticleVAO);
    glBindVertexArray(ParticleVAO);

    float Vertices[] = {
      // Vertices      TexCoords
        -0.05f, -0.05f,  0, 0,
        -0.05f,  0.05f,  0, 1,
         0.05f, -0.05f,  1, 0,
         0.05f,  0.05f,  1, 1
    };
    glGenBuffers(1, &ParticleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ParticleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    glUseProgram(ParticleShader.ShaderProgram);
    glBindVertexArray(ParticleVAO);
    glBindTexture(GL_TEXTURE_2D, Texture);
    SetMat4("ViewProjection", ParticleShader, Camera->ViewProjection);

    for (int i = 0; i < Particles.size(); i++)
    {
        // TODO(insolence): Probably map the Lifetime to a 0-1 range
        SetFloat("Lifetime", ParticleShader, Particles[i].Lifetime > 1.f ? 1.f : Particles[i].Lifetime); // NOTE(insolence): If lifetime of a particle is > 1 sec, round it to 1 sec
        SetVec2("Offset", ParticleShader, Particles[i].Position);
        //SetFloat("Size", ParticleShader, Particles[i].Size);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &ParticleVAO);
    glDeleteBuffers(1, &ParticleVBO);
}

void
DrawTriangleTextured(orthographic_camera *Camera, mat4 Transform, uint Texture, color Color = {0.f, 0.f, 0.f})
{
    unsigned int TexturedVAO = 0, TexturedVBO = 0;
    glGenVertexArrays(1, &TexturedVAO);
    glBindVertexArray(TexturedVAO);

    float Vertices[] = {
        // Vertices   TexCoords
        -1.0, 0.0,    0.0, 0.0,
         0.0, 1.0,    0.5, 1.0,
         1.0, 0.0,    1.0, 0.0
    };
    glGenBuffers(1, &TexturedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    glUseProgram(TexturedShader.ShaderProgram);
    glBindVertexArray(TexturedVAO);
    glBindTexture(GL_TEXTURE_2D, Texture);
    SetColor("CustomColor", TexturedShader, Color);
    SetMat4("ViewProjection", TexturedShader, Camera->ViewProjection);
    SetMat4("Transform", TexturedShader, Transform);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &TexturedVAO);
    glDeleteBuffers(1, &TexturedVBO);
}

void
RenderText(std::string Text, float X, float Y, float Scale, color Color)
{
    uint TextVAO = 0, TextVBO = 0;
    if (!TextVAO)
    {
        glGenVertexArrays(1, &TextVAO);
        glGenBuffers(1, &TextVBO);
        glBindVertexArray(TextVAO);
        glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    mat4 Projection = Ortho(0.f, SCREEN_WIDTH, 0.f, SCREEN_WIDTH, 0.f, 1.f);

    glUseProgram(TextShader.ShaderProgram);
    SetVec3("TextColor", TextShader, Color);
    SetMat4("Projection", TextShader, Projection);
    glBindVertexArray(TextVAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = Text.begin(); c != Text.end(); c++)
    {
        Character Ch = Characters[*c];

        GLfloat Xpos = X + Ch.Bearing.X * Scale;
        GLfloat Ypos = Y - (Ch.Size.Y - Ch.Bearing.Y) * Scale;

        GLfloat W = Ch.Size.X * Scale;
        GLfloat H = Ch.Size.Y * Scale;
        // Update VBO for each character
        GLfloat Vertices[6][4] = {
            { Xpos,     Ypos + H,   0.0, 0.0 },
            { Xpos,     Ypos,       0.0, 1.0 },
            { Xpos + W, Ypos,       1.0, 1.0 },

            { Xpos,     Ypos + H,   0.0, 0.0 },
            { Xpos + W, Ypos,       1.0, 1.0 },
            { Xpos + W, Ypos + H,   1.0, 0.0 }
        };
        glBindTexture(GL_TEXTURE_2D, Ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        // NOTE(insolence): Advance cursors for next glyph
        X += (Ch.Advance >> 6) * Scale; // NOTE(insolence): Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    glDeleteVertexArrays(1, &TextVAO);
    glDeleteBuffers(1, &TextVBO);
}

void
RenderScreenTexture()
{
    unsigned int ScreenVAO = 0, ScreenVBO = 0;

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

    glBindVertexArray(ScreenVAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &ScreenVAO);
    glDeleteBuffers(1, &ScreenVBO);
}

void
PostprocessScreenTexture(int ScreenTexture, postprocessing_effects Effects)
{
    // TODO(insolence): Handle the case when no Effects are set and we don't have to call FBShsader
    // if (!Effects.Inversion && !Effects.Grayscale && !Effects.Blur)
    //     return;

    glUseProgram(PostprocessingShader.ShaderProgram);
    SetBool("Inversion", PostprocessingShader, Effects.Inversion);
    SetBool("Grayscale", PostprocessingShader, Effects.Grayscale);
    SetBool("Blur", PostprocessingShader, Effects.Blur);
    glBindTexture(GL_TEXTURE_2D, ScreenTexture);

    RenderScreenTexture();
}

void
ApplyHDR(int ScreenTexture, int BloomTexture, float Exposure)
{
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

void
Blur(framebuffer *HdrFB, framebuffer *PingpongFB)
{
    // NOTE(insolence): Blur bright fragments with 2-pass Gaussian blur
    int Horizontal = 1;
    bool FirstIteration = true;
    int Amount = 20;
    for (int i = 0; i < Amount; i++)
    {
        glUseProgram(BlurShader.ShaderProgram);
        glBindFramebuffer(GL_FRAMEBUFFER, PingpongFB[Horizontal].ID);
        SetInt("Image", BlurShader, 0);
        SetInt("Horizontal", BlurShader, Horizontal);
        glBindTexture(GL_TEXTURE_2D, FirstIteration ? HdrFB->TextureAttachment[1] : PingpongFB[!Horizontal].TextureAttachment[0]);
        RenderScreenTexture();
        Horizontal = !Horizontal;
        if (FirstIteration)
            FirstIteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

///////////////////////////////////////
// NOTE(insolence): Instanced rendering
///////////////////////////////////////

void
InstancedDrawRectanglesTextured(orthographic_camera *Camera, mat4 *Transforms, int Amount, uint Texture, color Color = {0.f, 0.f, 0.f})
{
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
    glBindTexture(GL_TEXTURE_2D, Texture);
    SetColor("CustomColor", InstancedShader, Color);
    SetMat4("ViewProjection", InstancedShader, Camera->ViewProjection);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Amount);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &InstancedVAO);
    glDeleteBuffers(1, &InstancedVBO);
    glDeleteBuffers(1, &TransformsVBO);
}

void
InstancedDrawTrianglesTextured(orthographic_camera *Camera, mat4 *Transforms, int Amount, uint Texture, color Color = {0.f, 0.f, 0.f})
{
    unsigned int InstancedVAO = 0, InstancedVBO = 0;

    glGenVertexArrays(1, &InstancedVAO);
    glBindVertexArray(InstancedVAO);

    float Vertices[] = {
        // Vertices   TexCoords
        -1.0, 0.0,    0.0, 0.0,
         0.0, 1.0,    0.5, 1.0,
         1.0, 0.0,    1.0, 0.0
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
    glBindTexture(GL_TEXTURE_2D, Texture);
    SetColor("CustomColor", InstancedShader, Color);
    SetMat4("ViewProjection", InstancedShader, Camera->ViewProjection);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, Amount);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &InstancedVAO);
    glDeleteBuffers(1, &InstancedVBO);
    glDeleteBuffers(1, &TransformsVBO);
}
