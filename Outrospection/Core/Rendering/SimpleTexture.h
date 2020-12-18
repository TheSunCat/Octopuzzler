#pragma once

#include <string>

#include <GLAD/glad.h>

class SimpleTexture
{
public:
    SimpleTexture() = default;

    SimpleTexture(const GLuint& _texId);

    void bind() const;

    virtual void tick();

    GLuint texId = 0;

    bool operator==(const SimpleTexture& st) const;

    virtual ~SimpleTexture() = default;
};
