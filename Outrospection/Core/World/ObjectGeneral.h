#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Core/Rendering/Mesh.h"

class Shader;

class ObjectGeneral
{
public:
    ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale);

    void draw(const Shader& shader) const;

    glm::vec3 getPos() const;

    glm::vec3 getRot() const;

    glm::vec3 getScale() const;

protected:
    glm::vec3 pos{}, rot{}, scale{};
    std::vector<Mesh> meshes;

    std::string name;
};
