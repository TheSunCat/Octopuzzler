#pragma once

#include <unordered_map>

#include <GLAD/glad.h>

#include "Resource.h"
#include "Types.h"
#include "Constants.h"

#include "Core/Rendering/SimpleTexture.h"
#include "Core/Rendering/TickableTexture.h"

class TextureManager {
private:
	std::unordered_map<Resource, SimpleTexture, Hashes> textures;
	std::vector<TickableTexture> tickableTextures;
public:
	TextureManager();

	SimpleTexture loadTexture(Resource& r);

	TickableTexture loadAnimatedTexture(Resource& r, unsigned int textureTickLength, unsigned int textureFrameCount);

	void bindTexture(Resource& r);

	SimpleTexture get(Resource& r);

	void tickAllTextures();

	static SimpleTexture missingTexture;

	DISALLOW_COPY_AND_ASSIGN(TextureManager)
private:
	GLuint textureFromFile(const std::string& filename);
	void createTexture(const GLuint& texId, const unsigned char* data, const GLenum& format,
	                   const unsigned int& width, const unsigned int& height);
};