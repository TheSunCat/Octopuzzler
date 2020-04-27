#pragma once

#include <string>
#include <sstream>

class Resource
{
public:
	Resource(std::string path, std::string name);
	
	std::string getResourcePath() const;

	bool operator== (const Resource& r) const;

	std::string resourcePath;
	std::string resourceName;
};