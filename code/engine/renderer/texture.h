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
    texture Texture;
    Texture.FilteringMode = FilteringMode;
    Texture.WrappingMode = WrappingMode;

    glGenTextures(1, &Texture.ID);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrappingMode);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrappingMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilteringMode);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(Path, &width, &height, &nrChannels, 0);
    if (data)
    {
        // TODO(insolence): Add RGBA, sRGB loading
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // NOTE(insolence): Now only RGB support
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load a texture!");
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture;
}