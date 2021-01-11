#pragma once
#include <glm/glm.hpp>

class ObjectGeneral;

struct ManifoldPoints { // prev. feat. as CollisionPoints
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 n;
    float penetrationDepth;
    bool hasCollision;
};

struct Manifold
{
    ObjectGeneral* objA;
    ObjectGeneral* objB;

    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 n;

    float penetrationDepth;

    bool hasCollision;

    Manifold(
        ObjectGeneral* a,
        ObjectGeneral* b,
        const ManifoldPoints& points)
        : objA(a)
        , objB(b)
        , a(points.a)
        , b(points.b)
        , n(points.n)
        , penetrationDepth(points.penetrationDepth)
        , hasCollision(points.hasCollision)
    {}

};
