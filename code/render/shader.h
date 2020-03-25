#pragma once

struct shader
{
    int ShaderProgram;
    hash_map<const char*, uint> *UniformCache;
};

shader Shader;
shader TexturedAmbientShader;
shader TexturedDiffuseShader;
shader PostprocessingShader;
shader HDRShader;
shader InstancedShader;
shader BlurShader;
shader TextShader;
shader ParticleShader;

shader GuiTexturedShader;
shader GuiBasicShader;

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

// NOTE(insolence): Now takes paths such as: "shaders/basic.vert"
shader
CreateShader(const char *VertPath, const char *FragPath)
{
    shader Shader;
    Shader.UniformCache = CreateHashMap<const char*, uint>();

    const char *VertSrc = (const char *)ReadFile(VertPath);
    const char *FragSrc = (const char *)ReadFile(FragPath);

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
    free((void *)VertSrc);
    free((void *)FragSrc);

    Shader.ShaderProgram = ShaderProgram;

    return Shader;
}

void
DeleteShader(shader *Shader)
{
    glDeleteProgram(Shader->ShaderProgram);
    DeleteHashMap(Shader->UniformCache);
}

// NOTE(insolence): Tries to get the location from UniformCache,
// otherwise queries it from OpenGL and adds to the cache
internal int
GetLocation(const char *Name, shader Shader)
{
    uint Location;
    // if (Get(Shader.UniformCache, Name) == NULL)
    // {
    //     Location = glGetUniformLocation(Shader.ShaderProgram, Name);
    //     Insert(Shader.UniformCache, Name, Location);
    // }
    // else
    // {
    //    Location = Get(Shader.UniformCache, Name);
    // }

    Location = glGetUniformLocation(Shader.ShaderProgram, Name);

    return Location;
}

void
SetColor(const char *Name, shader Shader, color Color)
{
    int Location = GetLocation(Name, Shader);
    glUniform3f(Location, Color.R, Color.G, Color.B);
}

void
SetInt(const char *Name, shader Shader, int Value)
{
    int Location = GetLocation(Name, Shader);
    glUniform1i(Location, Value);
}

void
SetBool(const char *Name, shader Shader, bool Value)
{
    int Location = GetLocation(Name, Shader);
    glUniform1i(Location, (bool)Value);
}

void
SetFloat(const char *Name, shader Shader, float Value)
{
    int Location = GetLocation(Name, Shader);
    glUniform1f(Location, Value);
}

void
SetMat4(const char *Name, shader Shader, mat4 Matrix)
{
    int Location = GetLocation(Name, Shader);
    glUniformMatrix4fv(Location, 1, GL_FALSE, &Matrix.Elements[0]);
}

void
SetVec3(const char *Name, shader Shader, vec3 Vector)
{
    int Location = GetLocation(Name, Shader);
    glUniform3f(Location, Vector.X, Vector.Y, Vector.Z);
}

void
SetVec4(const char *Name, shader Shader, vec4 Vector)
{
    int Location = GetLocation(Name, Shader);
    glUniform4f(Location, Vector.X, Vector.Y, Vector.Z, Vector.W);
}

// NOTE(insolence): The Name var seems to be bugged, prob because of char length
void
SetVec2(const char *Name, shader Shader, vec2 Vector)
{
    int Location = GetLocation(Name, Shader);
    glUniform2f(Location, Vector.X, Vector.Y);
}

void
SetDirLight(int i, shader Shader, dir_light Light)
{
    char Buffer[40];

    sprintf(Buffer, "DirLights[%i].Direction", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Direction.X, Light.Direction.Y, Light.Direction.Z);

    sprintf(Buffer, "DirLights[%i].Ambient", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Ambient.X, Light.Ambient.Y, Light.Ambient.Z);

    sprintf(Buffer, "DirLights[%i].Diffuse", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Diffuse.X, Light.Diffuse.Y, Light.Diffuse.Z);

    sprintf(Buffer, "DirLights[%i].Specular", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Specular.X, Light.Specular.Y, Light.Specular.Z);
}

void
SetPointLight(int i, shader Shader, point_light Light)
{
    char Buffer[40];

    sprintf(Buffer, "PointLights[%i].Position", i);
    SetVec3(Buffer, Shader, Light.Position);

    sprintf(Buffer, "PointLights[%i].Ambient", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Ambient.X, Light.Ambient.Y, Light.Ambient.Z);

    sprintf(Buffer, "PointLights[%i].Diffuse", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Diffuse.X, Light.Diffuse.Y, Light.Diffuse.Z);

    sprintf(Buffer, "PointLights[%i].Specular", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Specular.X, Light.Specular.Y, Light.Specular.Z);

    //sprintf(Buffer, "PointLights[%i].CLQ", i);
    //glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.CLQ.X, Light.CLQ.Y, Light.CLQ.Z);
    //SetVec3(Buffer, Shader, Light.CLQ);
}

void
SetSpotLight(int i, shader Shader, spotlight_light Light)
{
    char Buffer[40];

    sprintf(Buffer, "SpotLights[%i].Position", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Position.X, Light.Position.Y, Light.Position.Z);

    sprintf(Buffer, "SpotLights[%i].Direction", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Direction.X, Light.Direction.Y, Light.Direction.Z);

    sprintf(Buffer, "SpotLights[%i].CutOff", i);
    SetFloat(Buffer, Shader, Light.CutOff);

    sprintf(Buffer, "SpotLights[%i].OuterCutOff", i);
    SetFloat(Buffer, Shader, Light.OuterCutOff);

    sprintf(Buffer, "SpotLights[%i].Ambient", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Ambient.X, Light.Ambient.Y, Light.Ambient.Z);

    sprintf(Buffer, "SpotLights[%i].Diffuse", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Diffuse.X, Light.Diffuse.Y, Light.Diffuse.Z);

    sprintf(Buffer, "SpotLights[%i].Specular", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.Specular.X, Light.Specular.Y, Light.Specular.Z);

    sprintf(Buffer, "PointLights[%i].CLQ", i);
    glUniform3f(glGetUniformLocation(Shader.ShaderProgram, Buffer), Light.CLQ.X, Light.CLQ.Y, Light.CLQ.Z);
}


internal void
MakeShaders()
{
    if (Shader.ShaderProgram)
    {
        DeleteShader(&Shader);
        DeleteShader(&TexturedDiffuseShader);
        DeleteShader(&TexturedAmbientShader);
        DeleteShader(&PostprocessingShader);
        DeleteShader(&HDRShader);
        DeleteShader(&InstancedShader);
        DeleteShader(&BlurShader);
        DeleteShader(&TextShader);
        DeleteShader(&ParticleShader);
        DeleteShader(&GuiTexturedShader);
        DeleteShader(&GuiBasicShader);
    }

    Shader = CreateShader("shaders/basic.vert", "shaders/basic.frag");
    TexturedAmbientShader = CreateShader("shaders/texture_ambient.vert", "shaders/texture_ambient.frag");
    TexturedDiffuseShader = CreateShader("shaders/texture_diffuse.vert", "shaders/texture_diffuse.frag");
    PostprocessingShader = CreateShader("shaders/postprocessing.vert", "shaders/postprocessing.frag");
    ParticleShader = CreateShader("shaders/particle.vert", "shaders/particle.frag");

    GuiTexturedShader = CreateShader("shaders/gui/texture.vert", "shaders/gui/texture.frag");
    GuiBasicShader = CreateShader("shaders/gui/basic.vert", "shaders/gui/basic.frag");

    HDRShader = CreateShader("shaders/hdr.vert", "shaders/hdr.frag");
    glUseProgram(HDRShader.ShaderProgram);
    SetInt("Scene", HDRShader, 1);
    SetInt("BloomBlur", HDRShader, 0);
    glUseProgram(0);

    InstancedShader = CreateShader("shaders/instanced.vert", "shaders/instanced.frag");
    BlurShader = CreateShader("shaders/blur.vert", "shaders/blur.frag");
    TextShader = CreateShader("shaders/text.vert", "shaders/text.frag");
}