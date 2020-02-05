#include <map>
#include <string>

struct Character
{
    GLuint TextureID;  // ID handle of the glyph texture
    vec2 Size;         // Size of glyph
    vec2 Bearing;      // Offset from baseline to left/top of glyph
    GLuint Advance;    // Offset to advance to next glyph
};
std::map<GLchar, Character> Characters;

void
LoadFreetype()
{
    if (!Characters.empty())
    {
        std::map<GLchar, Character>::iterator Itr;
        for (Itr = Characters.begin(); Itr != Characters.end(); Itr++)
        {
            // FIXME(insolence): For some reason this line doesn't work
            // However, we need to delete textures or we'll get memory leaks
            //glDeleteTextures(1, &(Itr->second.TextureID));
        }
        Characters.clear();
    }

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        printf("Freetype error: Could not init FreeType Library \n");

    FT_Face face;
    if (FT_New_Face(ft, "W:/Insosure-Engine/assets/fonts/arial.ttf", 0, &face))
        printf("Freetype error: Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // NOTE(insolence): Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++)
    {
        // NOTE(insolence): Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Freetype error: Failed to load Glyph \n");
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            vec2{ (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows},
            vec2{ (float)face->glyph->bitmap_left,  (float)face->glyph->bitmap_top},
            (GLuint)face->glyph->advance.x
            };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}