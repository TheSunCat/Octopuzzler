#include "Rigidbody.h"

#include "Util.h"

void Rigidbody::setLastTransformation(const Transform& lastTrans)
{
    m_lastTrans = lastTrans;
}

void Rigidbody::setNextTransformation(const Transform& nextTrans)
{
    m_nextTrans = nextTrans;
}

bool Rigidbody::isMoving() const
{
    return Util::length2(velocity) != 0.0f;
}

Rigidbody::Rigidbody(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider)
    : ObjectGeneral(_pos, _rot, _scale, _collider),
        mGravity(0, -9.8, 0), force(), velocity(), mass(1)
{
}

Rigidbody::Rigidbody(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Collider* _collider)
        : ObjectGeneral(_name, _pos, _rot, _scale, _collider),
            mGravity(0, -9.8, 0), force(), velocity(), mass(1)
{
    isRigidbody = true;
}

Rigidbody::Rigidbody(const std::string& _name, const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale, Mesh& mesh, Collider* _collider)
    : ObjectGeneral(_name, _pos, _rot, _scale, mesh, _collider),
    mGravity(0, -9.8, 0), force(), velocity(), mass(1)
{
    isRigidbody = true;
}

void Rigidbody::applyForce(const glm::vec3& _force)
{
    force += _force;
}

void Rigidbody::applyGravity()
{
    applyForce(mGravity * mass);
}

void Rigidbody::move(const glm::vec3& delta)
{
    transform.pos().get() += delta;
    velocity = delta;
}