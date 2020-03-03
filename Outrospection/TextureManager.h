#pragma once
#include "Resource.h"
#include <unordered_map>
#include "SimpleTexture.h"

class TextureManager {
public:
	TextureManager();

	void bindTexture(Resource r);
private:
	SimpleTexture loadTexture(Resource r);

	std::unordered_map<Resource, SimpleTexture> textures;
};