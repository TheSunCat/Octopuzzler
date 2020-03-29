#include "SimpleTexture.h"
#include <GLAD\glad.h>
#include <string>

SimpleTexture::SimpleTexture(const unsigned int& _texId, const std::string& _texPath)
{
	texId = _texId;
	texPath = _texPath;
}

void SimpleTexture::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, texId);
}
