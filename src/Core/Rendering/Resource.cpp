#include "Resource.h"

Resource::Resource(const std::string& path, const std::string& name) : fullPath("res/ObjectData/" + path + name)
{
}

std::string Resource::getResourcePath() const
{
    return fullPath;
}

bool Resource::operator==(const Resource& r) const
{
    return (fullPath == r.fullPath);
}
