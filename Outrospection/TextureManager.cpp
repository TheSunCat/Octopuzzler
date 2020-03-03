#include "TextureManager.h"
#include <GLAD\glad.h>
#include "stb_image.h"
#include <iostream>

TextureManager::TextureManager()
{

}

void TextureManager::bindTexture(Resource r)
{
	std::unordered_map<Resource, SimpleTexture>::const_iterator f = textures.find(r);

	SimpleTexture tex;

	if (f == textures.end()) { // resource not found in already existing storage, needs to be loaded
		tex = loadTexture(r);
	}
	else {
		tex = (*f).second;
	}

	tex.bindTexture();
}

SimpleTexture TextureManager::loadTexture(Resource r)
{
	std::string filename = r.getResourcePath();

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

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		SimpleTexture texObj(tex, r.getResourcePath());

		textures.insert(std::pair<Resource, SimpleTexture>(r, texObj));
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	//return tex;
}