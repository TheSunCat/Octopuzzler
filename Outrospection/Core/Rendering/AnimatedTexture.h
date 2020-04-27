#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>

#include "SimpleTexture.h"

class AnimatedTexture : public SimpleTexture
{
public:
	AnimatedTexture(const std::vector<unsigned int>& texIds, const std::string& _texPath, const unsigned int _frameLength);

	void step();

	void step(unsigned int& stepCount);

	void nextFrame();

private:
	const std::vector<unsigned int> textures;
	const unsigned int frameLength;

	unsigned int frameTally = 0;
	unsigned int curFrame = 0;
};