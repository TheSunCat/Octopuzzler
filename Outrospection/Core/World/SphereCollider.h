#pragma once
#include "Core/World/Collider.h"

class SphereCollider : public Collider
{
public:
    glm::vec3 mCenter;
    float mRadius;

    SphereCollider(const glm::vec3& center = glm::vec3(), float radius = 1.0f);

    Transform trans() const override;

    glm::vec3 findFurthestPoint(const Transform* transform, glm::vec3 direction) const override;

    ManifoldPoints testCollision(
        const Transform* transform,
        const Collider* collider,
        const Transform* colliderTransform) const override;

    ManifoldPoints testCollision(
        const Transform* transform,
        const SphereCollider* collider,
        const Transform* colliderTransform) const override;

    //CollisionPoints testCollision(
    //    const Transform* transform,
    //    const CapsuleCollider* collider,
    //    const Transform* colliderTransform) const override;

    //CollisionPoints testCollision(
    //    const Transform* transform,
    //    const PlaneCollider* collider,
    //    const Transform* colliderTransform) const override;
};
