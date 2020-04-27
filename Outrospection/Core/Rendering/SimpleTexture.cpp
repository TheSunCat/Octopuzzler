#include "SimpleTexture.h"

#include <GLAD\glad.h>

SimpleTexture::SimpleTexture(const unsigned int& _texId, const std::string& _texPath)
{
	texId = _texId;
	texPath = _texPath;
}

void SimpleTexture::bindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, texId);
}

bool SimpleTexture::operator== (const SimpleTexture& st) const
{
	return texId == st.texId;
}