#pragma once

#include <ft2build.h>

#include FT_FREETYPE_H

#include "Types.h"

class FreeType
{
public:
    void loadChar(FT_Face face, char c)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOG_ERROR("FreeType failed to load glyph \'%c\'!", c);
            return;
        }

        // create the texture for this character
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

        FontCharacter character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        loadedCharacters.insert(std::pair<char, FontCharacter>(c, character));
    }
	
    FreeType()
    {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            LOG_ERROR("Failed to initialize FreeType!");
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, "./res/ObjectData/UI/JamminTooSlow.otf", 0, &face))
        {
            LOG_ERROR("Failed to load JamminTooSlow.otf!");
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        for (unsigned char c = 'a'; c < 'a' + 26; c++)
        {
            loadChar(face, c);
        }
    	
        loadChar(face, ' ');
        loadChar(face, '^');
        loadChar(face, '_');
        loadChar(face, '<');
        loadChar(face, '>');

    	// arrows
        loadChar(face, 'U'); // up
        loadChar(face, 'D'); // down
        loadChar(face, 'L'); // left
        loadChar(face, 'R'); // right

        // eyes
        loadChar(face, 'N'); // none
        loadChar(face, 'C'); // circle
        loadChar(face, 'S'); // square
        loadChar(face, 'T'); // triangle
    }

    std::unordered_map<char, FontCharacter> loadedCharacters;
};
