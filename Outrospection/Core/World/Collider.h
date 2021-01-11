#pragma once

#include <glm/glm.hpp>

#include "Core/World/Manifold.h"

class SphereCollider;
struct Transform;


class Collider
{
public:
    Collider() = default;

    virtual Transform trans() const;

    virtual glm::vec3 findFurthestPoint(const Transform* transform, glm::vec3 direction) const;


    virtual ManifoldPoints testCollision(const Transform* trans,
                                          const Collider* collider,
                                          const Transform* colliderTransform) const = 0;

    virtual ManifoldPoints testCollision(const Transform* trans,
                                          const SphereCollider* collider,
                                          const Transform* colliderTransform) const = 0;

    //virtual CollisionPoints testCollision(const Transform* trans,
    //                                      const CapsuleCollider* collider,
    //                                      const Transform* colliderTransform) const = 0;

    //virtual CollisionPoints testCollision(const Transform* trans,
    //                                      const PlaneCollider* collider,
    //                                      const Transform* colliderTransform) const = 0;
};
