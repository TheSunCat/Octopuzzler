#pragma once

#include <string>

class SimpleTexture
{
public:
	SimpleTexture() = default;

	SimpleTexture(const unsigned int& _texId, const std::string& _texPath);

	void bindTexture() const;

	unsigned int texId = 0;

	bool operator== (const SimpleTexture& st) const;
private:
	std::string texPath;
};