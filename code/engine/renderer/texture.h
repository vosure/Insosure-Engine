#pragma once

global_variable hash_map<const char*, uint> *TextureCache;

uint
CreateTexture(const char *Path, int FilteringMode, int WrappingMode)
{
    // TODO(insolence): Size must be determined dynamically!!!
    char Append[120];
	strcpy(Append, "W:/Insosure-Engine/assets/textures/");
	strcat(Append, Path);

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
    unsigned char *Data = stbi_load(Append, &Width, &Height, &Channels, 0);

    //printf("Width: %d, Height: %d, Channels: %d \n", Width, Height, Channels);

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
    }
    else
    {
        printf("Failed to load a texture! ");
        printf("Name: %s \n", Path);
    }

    stbi_image_free(Data);
    glBindTexture(GL_TEXTURE_2D, 0);

    Insert(TextureCache, Path, Texture);

    return Texture;
}

// NOTE(insolence): Can return -1
uint GetTexture(const char *Path)
{
    // Query the TextureCache first
    if (Get(TextureCache, Path) != NULL)
    {
        return Get(TextureCache, Path);
    }

    return CreateTexture(Path, GL_NEAREST, GL_CLAMP_TO_EDGE);
}

// NOTE(insolence): At window resize
// TODO(insolence): Probably we should reupload all textures currently loaded
void
UpdateTextureCache()
{
    hash_map<const char*, uint> *Temp = CreateHashMap<const char*, uint>();
    hash_map<const char*, uint> *Swap = Temp;
    Temp = TextureCache;
    TextureCache = Swap;

    for (int i = 0; i < Temp->Size; i++)
    {
        if (Temp->Nodes[i] != NULL)
        {
            glDeleteTextures(1, &(uint)(Temp->Nodes[i]->Value));

            //char *Name = Temp->Nodes[i]->Key;
            //printf("Texture: %s \n", Name);
            //CreateTexture(Name, GL_NEAREST, GL_CLAMP_TO_BORDER);
        }
    }

    DeleteHashMap(Temp);

    // for (int i = 0; i < TextureCache->Size; i++)
    // {
    //     if (TextureCache->Nodes[i] != NULL)
    //     {
    //         printf("Name: %s \n", TextureCache->Nodes[i]->Key);
    //     }
    // }
}