#pragma once
#include <string>

class SimpleTexture
{
public:
	SimpleTexture() = default;

	SimpleTexture(const unsigned int& _texId, const std::string& _texPath);

	void bindTexture();

	unsigned int texId;
	
private:
	std::string texPath;
};