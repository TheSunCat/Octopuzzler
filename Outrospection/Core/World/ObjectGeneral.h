#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Core/Rendering/Mesh.h"

class Collider;
class Camera;
class Shader;

class ObjectGeneral
{
public:
    ObjectGeneral(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider = nullptr);

    ObjectGeneral(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider = nullptr);

    ObjectGeneral(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Mesh& mesh, Collider* _collider = nullptr);

    void draw(const Shader& shader, const Camera& cam) const;

    bool isRigidbody = false;
    bool isStatic = false;

    float debugColor = 0.0f;

    bool hidden = false;

    Collider* collider;
    Transform transform;


    virtual ~ObjectGeneral() = default;
protected:

    std::vector<Mesh> meshes;

    std::string name;
};
