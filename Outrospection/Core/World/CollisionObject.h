#pragma once
#include "Types.h"

class Collider;

class CollisionObject
{
protected:
    Transform mTransform;
    Collider* mCollider;

public:
    CollisionObject();
};
