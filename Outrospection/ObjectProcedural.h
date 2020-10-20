#pragma once
#include "MeshSphere.h"
#include "Core/World/ObjectGeneral.h"

class ObjectProcedural : ObjectGeneral
{
public:
    ObjectProcedural(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale)
        : ObjectGeneral(_name, _pos, _rot, _scale)
    {
        meshes.push_back(MeshSphere(5.0f, 1));
    }
};
