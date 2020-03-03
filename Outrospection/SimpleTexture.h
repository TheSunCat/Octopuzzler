#pragma once
class SimpleTexture
{
public:
	SimpleTexture() = default;

	SimpleTexture(unsigned int _texId, std::string _texPath);

	void bindTexture();

	unsigned int texId;
	
private:
	std::string texPath;
};