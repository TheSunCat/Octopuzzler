#include "TextureManager.h"

#include <iostream>

#include "../../External/stb_image.h"

TextureManager::TextureManager()
{
	const unsigned char missingTexData[] = {
		255, 0, 220, // purple, top left
		0,   0,   0, // black, top right
		0,   0,   0, // black, bottom left
		255, 0, 220  // purple, bottom right
	};

	unsigned int missingTexId = createTexture(missingTexData, GL_RGB, 2, 2);

	missingTexture = SimpleTexture(missingTexId, "missing_texture");
}

SimpleTexture TextureManager::loadTexture(Resource& r)
{
	std::string path = r.getResourcePath();

	unsigned int texId = textureFromFile(path);

	if (texId != -1) {
		SimpleTexture texObj(texId, path);

		textures.insert(std::pair<Resource, SimpleTexture>(r, texObj));

		return texObj;
	}
	else {
		std::cout << "Failed to generate texture ID for " << path << std::endl;

		return missingTexture;
	}
}

AnimatedTexture TextureManager::loadAnimatedTexture(Resource& r, unsigned int textureTickLength, unsigned int textureFrameCount)
{
	std::string path = r.getResourcePath();

	std::vector<unsigned int> textureIds;

	for (int i = 0; i < textureFrameCount; i++)
	{
		std::stringstream ss(path);
		ss << i << ".png";
		std::string currentPath = ss.str();

		unsigned int currentTextureId = textureFromFile(currentPath);


		if (currentTextureId != -1) {
			textureIds.push_back(currentTextureId);
		}
		else {
			std::cerr << "Failed to generate texture ID for " << path << std::endl;

			textureIds.push_back(missingTexture.texId);
		}
	}

	AnimatedTexture tex(textureIds, path, textureTickLength);

	textures.insert(std::pair<Resource, AnimatedTexture>(r, tex));

	tickableTextures.push_back(tex);

	return tex;
}

void TextureManager::bindTexture(Resource& r)
{
	SimpleTexture tex = get(r);

	tex.bindTexture();
}

SimpleTexture TextureManager::get(Resource& r)
{
	std::unordered_map<Resource, SimpleTexture>::const_iterator f = textures.find(r);

	SimpleTexture tex;

	if (f == textures.end()) { // resource not found in already existing storage, needs to be loaded
		tex = loadTexture(r);
	}
	else {
		tex = (*f).second;
	}

	return tex;
}

// Called every tick, calls step on every tickable texture.
void TextureManager::tickAllTextures()
{
	for (int i = 0; i < tickableTextures.size(); i++)
	{
		tickableTextures[i].step();
	}
}

unsigned int TextureManager::createTexture(const unsigned char* data, const GLenum& format, const unsigned int& width, const unsigned int& height)
{
	unsigned int tex = -1;

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

unsigned int TextureManager::textureFromFile(const std::string& filename)
{
	unsigned int tex;
	int width, height;
	glGenTextures(1, &tex);

	int nrComponents = 0;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		tex = createTexture(data, format, width, height);

		stbi_image_free(data);

		return tex;
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);

		return -1;
	}
}