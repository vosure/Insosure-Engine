
/*
NOTE(insolence): The coord space of GuiRendering is (-1, 1, -1, 1), X and Y respectively
*/

// TODO(insolence): SCALE!!!

void
DrawTexturedRectangleOnScreen(vec2 From, vec2 To, uint Texture)
{
    glDisable(GL_BLEND);

    unsigned int VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    From.X *= RESOLUTION;
    To.X   *= RESOLUTION;

    float Vertices[] = {
        From.X, From.Y, 0.f, 0.f,
        From.X, To.Y,   0.f, 1.f,
        To.X,   From.Y, 1.f, 0.f,
        To.X,   To.Y,   1.f, 1.f
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    mat4 Projection = Ortho(-1.f, 1.f, -RESOLUTION, RESOLUTION, 0.f, 1.f);


    glBindVertexArray(VAO);
    glUseProgram(GuiTexturedShader.ShaderProgram);
    SetMat4("Projection", GuiTexturedShader, Projection);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void
DrawRectangleOnScreen(vec2 From, vec2 To, vec3 Color)
{
    glDisable(GL_BLEND);

    unsigned int VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    From.X *= RESOLUTION;
    To.X   *= RESOLUTION;

    float Vertices[] = {
        From.X, From.Y,
        From.X, To.Y,
        To.X,   From.Y,
        To.X,   To.Y
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    mat4 Projection = Ortho(-1.f, 1.f, -RESOLUTION, RESOLUTION, 0.f, 1.f);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(VAO);
    glUseProgram(GuiBasicShader.ShaderProgram);
    SetMat4("Projection", GuiBasicShader, Projection);
    SetVec3("Color", GuiBasicShader, Color);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// TODO(insolence): Make this from -1 to 1 range, now it's in pixels(?)
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