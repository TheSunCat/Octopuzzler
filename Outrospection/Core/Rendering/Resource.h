#pragma once

#include <string>

class Resource
{
public:
    Resource(const std::string& path, const std::string& name);

    std::string getResourcePath() const;

    bool operator==(const Resource& r) const;

    const std::string fullPath;
};
