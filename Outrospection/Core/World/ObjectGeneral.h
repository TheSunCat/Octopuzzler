#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Core/Rendering/Mesh.h"

class Shader;

class ObjectGeneral
{
public:
    ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale);

    ObjectGeneral(const std::string& _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale, Mesh& mesh);

    void draw(const Shader& shader) const;

    glm::vec3& getPos();
    glm::vec3 getRot() const;
    glm::vec3& getRotRad();
    glm::vec3& getScale();

    void setPos(const glm::vec3& _pos);
    void setRot(const glm::vec3& _rot);
    void setScale(const glm::vec3& _scl);

    float debugColor;

    bool hidden = false;
protected:
    glm::vec3 pos{}, rotRad{}, scale{};
    glm::quat rot;

    mutable glm::mat4 modelMat;
    mutable bool dirtyTransform = true;

    std::vector<Mesh> meshes;

    std::string name;
};
