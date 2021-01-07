#pragma once
#include "Core/World/CollisionObject.h"

class RigidBody : public CollisionObject
{
private:
    glm::vec3 mGravity;
    glm::vec3 mForce;
    glm::vec3 mVelocity;


    float mass;

public:
    RigidBody();

    void applyForce(const glm::vec3& force);
    void applyGravity();
};