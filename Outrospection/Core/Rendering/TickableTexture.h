#pragma once

#include <vector>
#include <string>

#include "Core/Rendering/SimpleTexture.h"

class TickableTexture : public SimpleTexture
{
public:
	TickableTexture(const std::vector<unsigned int>& texIds, const std::string& _texPath, const unsigned int _frameLength);

	void step();

	void step(unsigned int& stepCount);

	void nextFrame();

private:
	const std::vector<unsigned int> textures;
	const unsigned int frameLength;

	unsigned int frameTally = 0;
	unsigned int curFrame = 0;
};