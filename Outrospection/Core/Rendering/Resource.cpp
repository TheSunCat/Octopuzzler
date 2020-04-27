#include "Resource.h"

Resource::Resource(std::string path, std::string name) {
	resourcePath = path;
	resourceName = name;
}

std::string Resource::getResourcePath() const {
	std::stringstream ss;
	ss << "./res/ObjectData/" << resourcePath << resourceName;
	return ss.str();
}

bool Resource::operator== (const Resource& r) const
{
	return (resourcePath == r.resourcePath && resourceName == r.resourceName);
}