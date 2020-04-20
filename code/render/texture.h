#pragma once

#include <unordered_map>

std::unordered_map<std::string, uint> TextureCache;

//global_variable hash_map<std::string, uint> *TextureCache;

uint
CreateTexture(std::string Path, int FilteringMode, int WrappingMode)
{
    std::string FullPath = "W:/Insosure-Engine/assets/textures/" + Path;

    uint Texture;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrappingMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrappingMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilteringMode);

    stbi_set_flip_vertically_on_load(1);

    GLenum InternalFormat = 0, DataFormat = 0;

    int Width, Height, Channels;
    unsigned char *Data = stbi_load(FullPath.c_str(), &Width, &Height, &Channels, 0);

    if (Data)
    {
        if (Channels == 4)
        {
            InternalFormat = GL_SRGB_ALPHA;
            DataFormat = GL_RGBA;
        }
        else if (Channels == 3)
        {
            InternalFormat = GL_SRGB;
            DataFormat = GL_RGB;
        }
        else
        {
            printf("Channels != 3 or 4!");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, DataFormat, GL_UNSIGNED_BYTE, Data);
        // glGenerateMipmap(GL_TEXTURE_2D);

        TextureCache[Path] = Texture;

    }
    else
    {
        printf("Failed to load a texture! ");
        printf("Name: %s \n", FullPath.c_str());
    }

    stbi_image_free(Data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture;
}

uint
CreateNormalMapTexture(std::string Path)
{
    std::string FullPath = "W:/Insosure-Engine/assets/textures/normals/" + Path;

    uint Texture;
    glGenTextures(1, &Texture);
    int Width, Height, Channels;

    unsigned char *Data = stbi_load(FullPath.c_str(), &Width, &Height, &Channels, 0);

    if (Data)
    {
        GLenum Format = 0;
        if (Channels == 4)
        {
            Format = GL_RGBA;
        }
        else if (Channels == 3)
        {
            Format = GL_RGB;
        }
        else
        {
            printf("Channels != 3 or 4!");
        }

        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
        glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        TextureCache[Path] = Texture;

        stbi_image_free(Data);

    }
    else
    {
        printf("Failed to load a normal map! ");
        printf("Name: %s \n", FullPath.c_str());
        stbi_image_free(Data);
    }

    return Texture;
}

// NOTE(insolence): Can return -1
uint
GetTexture(std::string Path)
{
    if (TextureCache[Path])
        return TextureCache[Path];

    return CreateTexture(Path, GL_NEAREST, GL_CLAMP_TO_EDGE);
}

// NOTE(insolence): Can return -1
uint
GetNormal(std::string Path)
{
    if (TextureCache[Path])
        return TextureCache[Path];

    return CreateNormalMapTexture(Path);
}

// FIXME(insolence):
// NOTE(insolence): That's a very temporary and brute-force solution
uint
GetNormalFromTexture(std::string TextureStr)
{
    const char *TextureStrArr = TextureStr.c_str();

    char NormalTextureStr[80];
    int Index = 0;
    while (TextureStrArr[Index] != '.')
    {
        NormalTextureStr[Index] = TextureStrArr[Index];
        Index++;
    }
    NormalTextureStr[Index] = '\0';
    strcat(NormalTextureStr, "_normal.png");

    uint NormalTexture = GetNormal(NormalTextureStr);

    return NormalTexture;
}


// FIXME(insolence): Make this valid again!!!
// NOTE(insolence): At window resize
// TODO(insolence): Probably we should reupload all textures currently loaded
void
UpdateTextureCache()
{
    TextureCache.clear();
}

// void
// UpdateTextureCache()
// {
//     hash_map<std::string, uint> *Temp = CreateHashMap<std::string, uint>();
//     hash_map<std::string*, uint> *Swap = Temp;
//     Temp = TextureCache;
//     TextureCache = Swap;

//     for (int i = 0; i < Temp->Size; i++)
//     {
//         if (Temp->Nodes[i] != NULL)
//         {
//             glDeleteTextures(1, &(uint)(Temp->Nodes[i]->Value));

//             //char *Name = Temp->Nodes[i]->Key;
//             //printf("Texture: %s \n", Name);
//             //CreateTexture(Name, GL_NEAREST, GL_CLAMP_TO_BORDER);
//         }
//     }

//     DeleteHashMap(Temp);

//     // for (int i = 0; i < TextureCache->Size; i++)
//     // {
//     //     if (TextureCache->Nodes[i] != NULL)
//     //     {
//     //         printf("Name: %s \n", TextureCache->Nodes[i]->Key);
//     //     }
//     // }
// }