#pragma once
#include "Core/World/ObjectGeneral.h"

class Rigidbody : public ObjectGeneral
{
private:
    // TODO huh??
    Transform m_lastTrans; // Where the rigidbody was last step
    Transform m_nextTrans; // Where the rigidbody will be next step if there is no interference
    

public:
    void setLastTransformation(const Transform& lastTrans);
    void setNextTransformation(const Transform& nextTrans);

    glm::vec3 mGravity;
    glm::vec3 force;
    glm::vec3 velocity;

    float mass;
    constexpr float invMass() const { return 1 / mass; }
    bool isMoving() const;

    Rigidbody(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider = nullptr);
    Rigidbody(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider = nullptr);
    Rigidbody(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Mesh& mesh, Collider* _collider = nullptr);

    void applyForce(const glm::vec3& _force);
    void applyGravity();

    void move(const glm::vec3& delta);
};