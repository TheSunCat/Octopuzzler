#include "RigidBody.h"

RigidBody::RigidBody() : mGravity(), mForce(), mVelocity(), mass()
{

}

void RigidBody::applyForce(const glm::vec3& force)
{
    mForce += force;
}

void RigidBody::applyGravity()
{
    applyForce(mGravity * mass);
}
