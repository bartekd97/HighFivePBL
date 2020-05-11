#pragma once

#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Character.h"

class Font
{
public:
    Font(FT_Library ft, const char* path, int size = 28);
    const Character& GetCharacter(char c);
    bool IsInitialized();
    int GetSize();

private:
    std::map<char, Character> characters;
    bool initialized;
    int size;
};
