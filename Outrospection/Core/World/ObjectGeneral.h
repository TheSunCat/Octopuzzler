#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Core/Rendering/Mesh.h"

class Shader;

class ObjectGeneral
{
public:
    ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale);

    ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale, Mesh& mesh);

    void draw(const Shader& shader) const;

    glm::vec3& getPos() const;
    glm::vec3& getRot() const;
    glm::vec3& getScale() const;

    void setPos(const glm::vec3& _pos);
    void setRot(const glm::vec3& _rot);
    void setScale(const glm::vec3& _scl);

protected:
    glm::vec3 pos{}, rotRad{}, scale{};

    mutable glm::mat4 modelMat;
    mutable bool dirtyTransform = true;

    std::vector<Mesh> meshes;

    std::string name;
};
