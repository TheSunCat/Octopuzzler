#pragma once
#include <string>
#include <sstream>

class Resource
{
public:
	//Resource(std::string name);

	Resource(std::string path, std::string name);
	
	std::string getResourcePath();

private:
	std::string resourcePath;
	std::string resourceName;
};