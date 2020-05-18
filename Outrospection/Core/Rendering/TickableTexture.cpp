#include "TickableTexture.h"

TickableTexture::TickableTexture(const std::vector<unsigned int>& texIds, const std::string& _texPath, const unsigned int _frameLength)
	: SimpleTexture(texIds.at(0), _texPath), textures(texIds), frameLength(_frameLength)
{
	
}

void TickableTexture::step()
{
	frameTally++;

	if (frameTally > frameLength)
	{
		frameTally = 0;
		nextFrame();
	}
}

void TickableTexture::step(unsigned int& stepCount)
{
	for (unsigned int i = 0; i < stepCount; i++)
	{
		step();
	}
}

void TickableTexture::nextFrame()
{
	if (curFrame < (textures.size() - 1))
		curFrame++;
	else
		curFrame = 0;

	texId = textures.at(curFrame);
}