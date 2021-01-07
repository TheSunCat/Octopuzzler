#pragma once

#include <glm/glm.hpp>

class SphereCollider;
struct Transform;

struct CollisionPoints
{
    glm::vec3 A; // furthest point of A in B
    glm::vec3 B; // furthest point of B in A
    glm::vec3 n; // B - A normalized

    float depth; // length of B - A
    bool hasCollision;
};

class Collider
{
public:
    Collider();

    virtual Transform trans() const;

    virtual glm::vec3 findFurthestPoint(const Transform* transform, glm::vec3 direction) const;


    virtual CollisionPoints testCollision(const Transform* trans,
                                          const Collider* collider,
                                          const Transform* colliderTransform) const = 0;

    virtual CollisionPoints testCollision(const Transform* trans,
                                          const SphereCollider* collider,
                                          const Transform* colliderTransform) const = 0;

    //virtual CollisionPoints testCollision(const Transform* trans,
    //                                      const CapsuleCollider* collider,
    //                                      const Transform* colliderTransform) const = 0;

    //virtual CollisionPoints testCollision(const Transform* trans,
    //                                      const PlaneCollider* collider,
    //                                      const Transform* colliderTransform) const = 0;
};
