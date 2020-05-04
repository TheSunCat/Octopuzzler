#pragma once

#include <unordered_map>

#include "Resource.h"

#include "Core/Rendering/SimpleTexture.h"
#include "Core/Rendering/AnimatedTexture.h"

#include "Types.h"

class TextureManager {
private:
	std::unordered_map<Resource, SimpleTexture, Hashes> textures;
	std::vector<AnimatedTexture> tickableTextures;
public:
	TextureManager();

	SimpleTexture loadTexture(Resource& r);

	AnimatedTexture loadAnimatedTexture(Resource& r, unsigned int textureTickLength, unsigned int textureFrameCount);

	void bindTexture(Resource& r);

	SimpleTexture get(Resource& r);

	void tickAllTextures();

	static SimpleTexture missingTexture;
private:
	unsigned int textureFromFile(const std::string& filename);
	void createTexture(const unsigned int& texId, const unsigned char* data, const GLenum& format, const unsigned int& width, const unsigned int& height);
};