#pragma once 

#include "engine.h"

// TODO(insolence): Should shader be a separate struct? 
// Now it has nothing to hold except its program ID

internal void 
CheckCompileErrors(int Shader, const char *Type)
{
    int Success;
    char InfoLog[1024];
    if(Type != "PROGRAM")
    {
        glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
        if(!Success)
        {
            glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
            printf("Shader compilation error of type: %s \n", Type);
            printf("%s \n", InfoLog);
        }
    }
    else
    {
        glGetProgramiv(Shader, GL_LINK_STATUS, &Success);
        if(!Success)
        {
            glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
            printf("Program linking error of type: %s \n", Type);
            printf("%s \n", InfoLog);
        }
    }
}

int
CreateShaderProgram(const char *VertSrc, const char *FragSrc)
{
    int VertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertShader, 1, &VertSrc, NULL);
    glCompileShader(VertShader);
    CheckCompileErrors(VertShader, "VERTEX");

    int FragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragShader, 1, &FragSrc, NULL);
    glCompileShader(FragShader);
    CheckCompileErrors(FragShader, "FRAGMENT");


    int ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, VertShader);
    glAttachShader(ShaderProgram, FragShader);
    glLinkProgram(ShaderProgram);
    CheckCompileErrors(ShaderProgram, "PROGRAM");

    glDeleteShader(VertShader);
    glDeleteShader(FragShader);

    return ShaderProgram;
}

void 
SetColor(const char* Name, int ShaderProgram, color Color)
{ 
    glUniform3f(glGetUniformLocation(ShaderProgram, Name), Color.R, Color.G, Color.B); 
}

void 
SetInt(const char* Name, int ShaderProgram, int Value)
{ 
    glUniform1i(glGetUniformLocation(ShaderProgram, Name), Value); 
}

void 
SetBool(const char* Name, int ShaderProgram, bool Value)
{ 
    glUniform1i(glGetUniformLocation(ShaderProgram, Name), (bool)Value); 
}

void 
SetFloat(const char* Name, int ShaderProgram, float Value)
{ 
    glUniform1f(glGetUniformLocation(ShaderProgram, Name), Value); 
}

/////////////////////////////////////////////////////////////////
// TODO(insolence): Make Set functions for vec2, vec3, mat3, mat4 
