#pragma once
#include "Core.h"
#include "Types.h"

struct CollisionPackage;

class CollisionMesh
{
public:
    CollisionMesh(std::string& _name, std::vector <Triangle>& colData);

    void collide(CollisionPackage& package) const;

private:
    void collidesWith(CollisionPackage& package, const Triangle& tri) const;

    std::string name;
    std::vector<Triangle> collisionData;

};
