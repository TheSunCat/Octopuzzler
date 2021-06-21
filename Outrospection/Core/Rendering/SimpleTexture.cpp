#include "SimpleTexture.h"

SimpleTexture::SimpleTexture(const GLuint& _texId)
{
    texId = _texId;
}

void SimpleTexture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, texId);
}

void SimpleTexture::tick()
{
}

void SimpleTexture::reset()
{
}

bool SimpleTexture::operator==(const SimpleTexture& st) const
{
    return texId == st.texId;
}
