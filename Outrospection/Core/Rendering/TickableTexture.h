#pragma once

#include <vector>
#include <string>

#include "Core/Rendering/SimpleTexture.h"

class TickableTexture : public SimpleTexture
{
public:
	TickableTexture(const std::vector<GLuint>& texIds, const std::string& _texPath, const unsigned int _frameLength);

	void step();

	void step(unsigned int& stepCount);

	void nextFrame();

private:
	const std::vector<GLuint> textures;
	const unsigned int frameLength;

	unsigned int frameTally = 0;
	GLuint curFrame = 0;
};