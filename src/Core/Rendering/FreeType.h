#pragma once

#include <ft2build.h>

#include FT_FREETYPE_H

#include "Types.h"

class FreeType
{
public:
    void loadChar(FT_Face face, char c);

    FreeType();

    std::unordered_map<char, FontCharacter> loadedCharacters;
};
