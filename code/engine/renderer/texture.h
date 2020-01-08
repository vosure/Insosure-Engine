#pragma once

#include "..\..\..\dependencies\stb_image\stb_image.cpp"

struct texture
{
    unsigned int ID;
    float *TexCoords;
    int FilteringMode; // NOTE(insolence): GL_NEAREST or GL_LINEAR
    int WrappingMode;  // NOTE(insolence): GL_REPEAT, GL_CLAMP etc. 
};

texture 
CreateTexture(const char *Path, int FilteringMode, int WrappingMode)
{
    char Append[50];
	strcpy(Append, "D:/dev/InsosureEngine/assets/textures/");
	strcat(Append, Path);

    texture Texture;
    Texture.FilteringMode = FilteringMode;
    Texture.WrappingMode = WrappingMode;

    glGenTextures(1, &Texture.ID);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrappingMode);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrappingMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilteringMode);

    int Width, Height, NrChannels;
    unsigned char *Data = stbi_load(Append, &Width, &Height, &NrChannels, 0);
    if (Data)
    {
        // TODO(insolence): Add RGBA, sRGB loading
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data); // NOTE(insolence): Now only RGB support
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load a texture!");
    }

    stbi_image_free(Data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture;
}