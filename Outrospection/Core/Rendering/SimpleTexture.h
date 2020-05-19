#pragma once

#include <string>

#include <GLAD/glad.h>

class SimpleTexture
{
public:
	SimpleTexture() = default;

	SimpleTexture(const GLuint& _texId, const std::string& _texPath);

	void bindTexture() const;

	GLuint texId = 0;

	bool operator== (const SimpleTexture& st) const;
private:
	std::string texPath;
};
