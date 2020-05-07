#pragma once

#include <string>

class Resource
{
public:
	Resource(std::string path, std::string name);
	
	std::string getResourcePath() const;

	bool operator== (const Resource& r) const;

	std::string resourcePath;
	std::string resourceName;

	std::string fullPath;
};