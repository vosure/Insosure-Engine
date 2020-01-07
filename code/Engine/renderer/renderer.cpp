#include "renderer.h"
<<<<<<< HEAD
#include "texture.h"
=======
#include "../utils/file_utils.h"
>>>>>>> 442e3c63ce1f6c1d7c741b17ce67c6fa05665c59

// TODO: Write a normal file reading function
internal void
LoadShaderCode(const char* ShaderSrc, const char* ShaderCode)
{
    FILE* fp;
    fp = fopen(ShaderSrc, "r");

    fgets((char*)ShaderCode, 400, (FILE*)fp);

    fclose(fp);
}

internal int
CreateShaderProgram(const char *VertexSrc, const char *FragSrc)
{

    //NOTE(vosure) Relative path doesn't work, magic?
    //NOTE(vosure) free this pointer after you read it, otherwise we get a memory leak
    char *test_file_data = read_file("W:/Insosure-Engine/code/Engine/utils/test_file.txt");
    printf(test_file_data);
    // const char* VertexSrc = (const char*)malloc(sizeof(char) * 400);
    // const char* FragSrc = (const char*)malloc(sizeof(char) * 400);
    // LoadShaderCode("D:/dev/InsosureEngine/code/Engine/assets/shaders/basic.vert", VertexSrc);
    // LoadShaderCode("D:/dev/InsosureEngine/code/Engine/assets/shaders/basic.frag", FragSrc);

    int VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &VertexSrc, NULL);
    glCompileShader(VertexShader);
    int Success;
    char InfoLog[512];
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);
    if (!Success)
    {
        glGetShaderInfoLog(VertexShader, 512, NULL, InfoLog);
        printf("Vertex shader compilation failed!");    
    }

    int FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &FragSrc, NULL);
    glCompileShader(FragmentShader);
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Success);
    if (!Success)
    {
        glGetShaderInfoLog(FragmentShader, 512, NULL, InfoLog);
        printf("Fragment shader compilation failed!");
    }

    int ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, VertexShader);
    glAttachShader(ShaderProgram, FragmentShader);
    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, 512, NULL, InfoLog);
        printf("Shader linking failed failed!");
    }
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return ShaderProgram;
}

internal void 
SetColor(const char* Name, int ShaderProgram, color Color)
{ 
    glUniform3f(glGetUniformLocation(ShaderProgram, Name), Color.r, Color.g, Color.b); 
}

static unsigned int VAO = 0, VBO = 0, ShaderProgram = 0;
void 
DrawRectangle(float FromX, float FromY, float ToX, float ToY, color Color)
{
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

        const char* VertexSrc = "#version 330 core\n"
        "layout (location = 0) in vec2 Position;\n"
        "void main()\n"
        "{\n"
            "gl_Position = vec4(Position, 0.0, 1.0);\n"
        "}\0";

        const char* FragSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 Color;\n"
        "void main()\n"
        "{\n"
            "FragColor = vec4(Color, 1.0);\n"
        "}\0";

        ShaderProgram = CreateShaderProgram(VertexSrc, FragSrc);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glUseProgram(ShaderProgram);
    SetColor("Color", ShaderProgram, Color);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static unsigned int TexturedVAO = 0, TexturedVBO = 0, TexturedShaderProgram = 0;
void 
DrawRectangleTextured(float FromX, float FromY, float ToX, float ToY, texture Texture)
{
    if (!TexturedVAO)
    {
        float vertices[] = {
            // Vertices    // TexCoords
            FromX, FromY,  Texture.TexCoords[0], Texture.TexCoords[1], // TODO(Insolence): WTF is that
            FromX, ToY,    Texture.TexCoords[0], Texture.TexCoords[3],
            ToX,   FromY,  Texture.TexCoords[2], Texture.TexCoords[1],
            ToX,   ToY,    Texture.TexCoords[2], Texture.TexCoords[3],
        };

        glGenVertexArrays(1, &TexturedVAO);
        glGenBuffers(1, &TexturedVBO);

        glBindVertexArray(TexturedVAO);
        glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

        const char* VertexSrc = "#version 330 core\n"
        "layout (location = 0) in vec2 aPosition;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
            "gl_Position = vec4(aPosition, 0.0, 1.0);\n"
            "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}\0";

        const char* FragSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
            "FragColor = texture(ourTexture, TexCoord);\n"
        "}\0";

        TexturedShaderProgram = CreateShaderProgram(VertexSrc, FragSrc);
    }

    glBindVertexArray(TexturedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TexturedVBO);

    glUseProgram(TexturedShaderProgram);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}