#pragma once
#include "Resource.h"
#include <unordered_map>
#include <GLAD\glad.h>
#include "SimpleTexture.h"
#include "AnimatedTexture.h"

class TextureManager {
private:
	std::unordered_map<Resource, SimpleTexture*> textures;
	std::vector<AnimatedTexture*> tickableTextures;

	SimpleTexture missingTexture;
public:
	TextureManager();

	SimpleTexture loadTexture(Resource& r);

	AnimatedTexture loadAnimatedTexture(Resource& r, unsigned int textureTickLength, unsigned int textureFrameCount);

	void bindTexture(Resource& r);

	void tickAllTextures();
private:
	unsigned int TextureFromFile(const std::string& filename);
	unsigned int createTexture(const unsigned char* data, const GLenum& format, const unsigned int& width, const unsigned int& height);
};