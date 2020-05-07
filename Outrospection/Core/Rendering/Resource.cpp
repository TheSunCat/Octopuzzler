#include "Resource.h"

#include <sstream>
#include <utility>

Resource::Resource(std::string path, std::string name)
{
	resourcePath = std::move(path);
	resourceName = std::move(name);

	std::stringstream ss;
	ss << "./res/ObjectData/" << resourcePath << resourceName;
	fullPath = ss.str();
}

std::string Resource::getResourcePath() const {
	return fullPath;
}

bool Resource::operator== (const Resource& r) const
{
	return (resourcePath == r.resourcePath && resourceName == r.resourceName);
}