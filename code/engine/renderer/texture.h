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

    stbi_set_flip_vertically_on_load(1);

    GLenum InternalFormat = 0, DataFormat = 0;

    int Width, Height, Channels;
    unsigned char *Data = stbi_load(Append, &Width, &Height, &Channels, 0);

    printf("Width: %d, Height: %d, Channels: %d \n", Width, Height, Channels);

    if (Data)
    {
        if (Channels == 4)
        {
            InternalFormat = GL_RGBA8;
            DataFormat = GL_RGBA;
        }
        else if (Channels == 3)
        {
            InternalFormat = GL_RGB8;
            DataFormat = GL_RGB;
        }
        else
        {
            printf("Channels != 3 or 4!");
        }
        

        // TODO(insolence): Add RGBA, sRGB loading
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, DataFormat, GL_UNSIGNED_BYTE, Data); // NOTE(insolence): Now only RGB support
        // glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load a texture!");
    }

    stbi_image_free(Data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture;
}