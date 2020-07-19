#pragma once

#include <string>

#include <GLAD/glad.h>

class SimpleTexture
{
public:
    SimpleTexture() = default;

    SimpleTexture(const GLuint& _texId, const std::string& _texPath);

    void bind() const;

    virtual void tick();

    GLuint texId = 0;

    bool operator== (const SimpleTexture& st) const;

    virtual ~SimpleTexture() = default;
private:
    std::string texPath;
};
