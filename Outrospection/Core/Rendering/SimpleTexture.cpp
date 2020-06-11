#include "SimpleTexture.h"

SimpleTexture::SimpleTexture(const GLuint& _texId, const std::string& _texPath)
{
	texId = _texId;
	texPath = _texPath;
}

void SimpleTexture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, texId);
}

void SimpleTexture::tick() {}

bool SimpleTexture::operator== (const SimpleTexture& st) const
{
	return texId == st.texId;
}
