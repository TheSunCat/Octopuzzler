#pragma once

#include <unordered_map>

#include "Resource.h"

#include "SimpleTexture.h"
#include "AnimatedTexture.h"

#include "../../Types.h"

class TextureManager {
private:
	std::unordered_map<Resource, SimpleTexture, Hashes> textures;
	std::vector<AnimatedTexture> tickableTextures;

	SimpleTexture missingTexture;
public:
	TextureManager();

	SimpleTexture loadTexture(Resource& r);

	AnimatedTexture loadAnimatedTexture(Resource& r, unsigned int textureTickLength, unsigned int textureFrameCount);

	void bindTexture(Resource& r);

	SimpleTexture get(Resource& r);

	void tickAllTextures();
private:
	unsigned int textureFromFile(const std::string& filename);
	unsigned int createTexture(const unsigned char* data, const GLenum& format, const unsigned int& width, const unsigned int& height);
};