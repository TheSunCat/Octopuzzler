#include "Resource.h"

Resource::Resource(std::string path, std::string name) {
	resourcePath = path;
	resourceName = name;
}

std::string Resource::getResourcePath() {
	std::stringstream ss;
	ss << "./res/ObjectData/" << resourcePath << resourceName;
	return ss.str();
}