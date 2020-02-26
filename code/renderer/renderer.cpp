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
DrawRectangleTextured(orthographic_camera *Camera, mat4 Transform, uint Texture, uint NormalMap, std::vector<point_light> Lights, color Color = {0.f, 0.f, 0.f})
{
    if (!NormalMap)
    {
        NormalMap = GetNormal("default.png");
    }

    unsigned int TexturedVAO = 0, TexturedVBO = 0;

    // positions
    vec3 pos1 = {-1.0f,  1.0f, 0};
    vec3 pos2 = {-1.0f, -1.0f, 0};
    vec3 pos3 = { 1.0f, -1.0f, 0};
    vec3 pos4 = { 1.0f,  1.0f, 0};
    // texture coordinates
    vec2 uv1 = {0.0f, 1.0f};
    vec2 uv2 = {0.0f, 0.0f};
    vec2 uv3 = {1.0f, 0.0f};
    vec2 uv4 = {1.0f, 1.0f};
    // normal vector
    vec3 nm = {0.0f, 0.0f, 1.0f};

    // calculate tangent/bitangent vectors of both triangles
    vec3 tangent1, bitangent1;
    vec3 tangent2, bitangent2;

    // triangle 1
    // ----------
    vec3 edge1 = pos2 - pos1;
    vec3 edge2 = pos3 - pos1;
    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);
    tangent1.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
    tangent1.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
    tangent1.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
    tangent1 = Normalize(tangent1);

    bitangent1.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
    bitangent1.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
    bitangent1.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
    bitangent1 = Normalize(bitangent1);
    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;
    f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);
    tangent2.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
    tangent2.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
    tangent2.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
    tangent2 = Normalize(tangent2);
    bitangent2.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
    bitangent2.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
    bitangent2.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
    bitangent2 = Normalize(bitangent2);

    float quadVertices[] = {
        // positions            // normal         // texcoords  // tangent                          // bitangent
        pos1.X, pos1.Y, pos1.Z, nm.X, nm.Y, nm.Z, uv1.X, uv1.Y, tangent1.X, tangent1.Y, tangent1.Z, bitangent1.X, bitangent1.Y, bitangent1.Z,
        pos2.X, pos2.Y, pos2.Z, nm.X, nm.Y, nm.Z, uv2.X, uv2.Y, tangent1.X, tangent1.Y, tangent1.Z, bitangent1.X, bitangent1.Y, bitangent1.Z,
        pos3.X, pos3.Y, pos3.Z, nm.X, nm.Y, nm.Z, uv3.X, uv3.Y, tangent1.X, tangent1.Y, tangent1.Z, bitangent1.X, bitangent1.Y, bitangent1.Z,
        pos1.X, pos1.Y, pos1.Z, nm.X, nm.Y, nm.Z, uv1.X, uv1.Y, tangent2.X, tangent2.Y, tangent2.Z, bitangent2.X, bitangent2.Y, bitangent2.Z,
        pos3.X, pos3.Y, pos3.Z, nm.X, nm.Y, nm.Z, uv3.X, uv3.Y, tangent2.X, tangent2.Y, tangent2.Z, bitangent2.X, bitangent2.Y, bitangent2.Z,
        pos4.X, pos4.Y, pos4.Z, nm.X, nm.Y, nm.Z, uv4.X, uv4.Y, tangent2.X, tangent2.Y, tangent2.Z, bitangent2.X, bitangent2.Y, bitangent2.Z
    };

    // configure plane VAO
    glGenVertexArrays(1, &TexturedVAO);
    glGenBuffers(1, &TexturedVBO);
    glBindVertexArray(TexturedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

    glBindVertexArray(TexturedVAO);

    glUseProgram(TexturedDiffuseShader.ShaderProgram);
    glBindVertexArray(TexturedVAO);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture);
    SetInt("OurTexture", TexturedDiffuseShader, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, NormalMap);
    SetInt("NormalMap", TexturedDiffuseShader, 0);

    SetMat4("ViewProjection", TexturedDiffuseShader, Camera->ViewProjection);
    SetMat4("Transform", TexturedDiffuseShader, Transform);

    for (int i = 0; i < Lights.size(); i++)
    {
        SetPointLight(i, TexturedDiffuseShader, Lights[i]);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteVertexArrays(1, &TexturedVAO);
    glDeleteBuffers(1, &TexturedVBO);
}

// FIXME(insolence): Make this instanced rendering
void
DrawParticles(orthographic_camera *Camera, std::vector<particle> &Particles, uint Texture = 0)
{
    if (Texture == 0)
        Texture = GetTexture("blank.jpg");

    unsigned int ParticleVAO = 0, ParticleVBO = 0;
    glGenVertexArrays(1, &ParticleVAO);
    glBindVertexArray(ParticleVAO);

    glGenBuffers(1, &ParticleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ParticleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, NULL, GL_DYNAMIC_DRAW);

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
        float LifetimeLeft = Particles[i].Lifetime / Particles[i].InitialLifetime; // TODO(insolence): Mapping the Lifetime to a 0-1 range
        SetFloat("Lifetime", ParticleShader, LifetimeLeft);
        SetVec2("Offset", ParticleShader, Particles[i].Position);

        mat4 TransformMat = Transform(Particles[i].Position, Particles[i].Rotation, Particles[i].Size * LifetimeLeft);
        SetMat4("Transform", ParticleShader, TransformMat);

        color CurrentColor = Lerp(Particles[i].EndColor * 2, Particles[i].InitialColor, LifetimeLeft);
        SetColor("Color", ParticleShader, CurrentColor);

        float Vertices[16] = {
            -0.05f, -0.05f,  0, 0,
            -0.05f,  0.05f,  0, 1,
             0.05f, -0.05f,  1, 0,
             0.05f,  0.05f,  1, 1
        };
        glBindBuffer(GL_ARRAY_BUFFER, ParticleVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &ParticleVAO);
    glDeleteBuffers(1, &ParticleVBO);
}

// void
// DrawTriangleTextured(orthographic_camera *Camera, mat4 Transform, uint Texture, color Color = {0.f, 0.f, 0.f})
// {
//     unsigned int TexturedVAO = 0, TexturedVBO = 0;
//     glGenVertexArrays(1, &TexturedVAO);
//     glBindVertexArray(TexturedVAO);

//     float Vertices[] = {
//         // Vertices   TexCoords
//         -1.0, 0.0,    0.0, 0.0,
//          0.0, 1.0,    0.5, 1.0,
//          1.0, 0.0,    1.0, 0.0
//     };
//     glGenBuffers(1, &TexturedVBO);
//     glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

//     glUseProgram(TexturedShader.ShaderProgram);
//     glBindVertexArray(TexturedVAO);
//     glBindTexture(GL_TEXTURE_2D, Texture);
//     SetColor("CustomColor", TexturedShader, Color);
//     SetMat4("ViewProjection", TexturedShader, Camera->ViewProjection);
//     SetMat4("Transform", TexturedShader, Transform);

//     glDrawArrays(GL_TRIANGLES, 0, 3);

//     glBindVertexArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glUseProgram(0);
//     glBindTexture(GL_TEXTURE_2D, 0);

//     glDeleteVertexArrays(1, &TexturedVAO);
//     glDeleteBuffers(1, &TexturedVBO);
// }

void
RenderTextOnScreen(std::string Text, float X, float Y, float Scale, color Color)
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
    SetMat4("ViewProjection", TextShader, Projection);
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

// // FIXME(insolence): Fix this to work properly with world coords
// void
// RenderText(orthographic_camera *Camera, std::string Text, float X, float Y, float Scale, color Color)
// {
//     uint TextVAO = 0, TextVBO = 0;
//     if (!TextVAO)
//     {
//         glGenVertexArrays(1, &TextVAO);
//         glGenBuffers(1, &TextVBO);
//         glBindVertexArray(TextVAO);
//         glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);
//     }

//     X *= SCREEN_WIDTH;
//     Y *= SCREEN_HEIGHT;

//     glUseProgram(TextShader.ShaderProgram);
//     SetVec3("TextColor", TextShader, Color);
//     SetMat4("ViewProjection", TextShader, Camera->ViewProjection);
//     glBindVertexArray(TextVAO);

//     // Iterate through all characters
//     std::string::const_iterator c;
//     for (c = Text.begin(); c != Text.end(); c++)
//     {
//         Character Ch = Characters[*c];

//         GLfloat Xpos = (X + Ch.Bearing.X * Scale);
//         GLfloat Ypos = (Y - (Ch.Size.Y - Ch.Bearing.Y) * Scale);

//         GLfloat W = (Ch.Size.X * Scale);
//         GLfloat H = (Ch.Size.Y * Scale);
//         // Update VBO for each character
//         GLfloat Vertices[6][4] = {
//             { Xpos,     Ypos + H,   0.0, 0.0 },
//             { Xpos,     Ypos,       0.0, 1.0 },
//             { Xpos + W, Ypos,       1.0, 1.0 },

//             { Xpos,     Ypos + H,   0.0, 0.0 },
//             { Xpos + W, Ypos,       1.0, 1.0 },
//             { Xpos + W, Ypos + H,   1.0, 0.0 }
//         };
//         glBindTexture(GL_TEXTURE_2D, Ch.TextureID);
//         glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);

//         glDrawArrays(GL_TRIANGLES, 0, 6);
//         // NOTE(insolence): Advance cursors for next glyph
//         X += (Ch.Advance >> 6) * Scale; // NOTE(insolence): Bitshift by 6 to get value in pixels (2^6 = 64)
//     }
//     glBindVertexArray(0);
//     glBindTexture(GL_TEXTURE_2D, 0);
//     glUseProgram(0);

//     glDeleteVertexArrays(1, &TextVAO);
//     glDeleteBuffers(1, &TextVBO);
// }

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

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    //glActiveTexture(0);
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glActiveTexture(1);
    //glBindTexture(GL_TEXTURE_2D, 0);

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
    //SetInt("Scene", HDRShader, 2);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, ScreenTexture);
    //SetInt("BloomBlur", HDRShader, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, BloomTexture);
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

    glBindTexture(GL_TEXTURE_2D, 0);
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
