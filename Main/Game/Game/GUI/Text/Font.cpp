#include <glad/glad.h>

#include "Font.h"
#include "../../Utility/Logger.h"

Font::Font(FT_Library ft, const char* path, int size)
{
    initialized = false;
    FT_Face face;
	if (FT_New_Face(ft, path, 0, &face))
	{
		LogWarning("Font::Font(): Failed to load font {}", path);
		return;
	}
	FT_Set_Pixel_Sizes(face, 0, size);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LogWarning("Font::Font(): Failed to load glyph '{}'", c);
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    this->size = size;
    initialized = true;
}

bool Font::IsInitialized()
{
    return initialized;
}

const Character& Font::GetCharacter(char c)
{
    return characters[c];
}

int Font::GetSize()
{
    return size;
}
