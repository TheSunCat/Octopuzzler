#pragma once
#include <vector>

#include "ObjectGeneral.h"

struct Manifold;
class Solver;
class Collider;

class PhysicsWorld
{
protected:
    std::vector<ObjectGeneral*> mObjects;
    std::vector<Solver*> mSolvers;

public:
    PhysicsWorld();

    // the three steps of step(dt)
    void applyGravity();
    void predictTransforms(float deltaTime);
    void clearForces();

    void step(float deltaTime);


    void addCollisionObject(ObjectGeneral* theObject);
    void removeCollisionObject(ObjectGeneral* theObject);

    void addSolver(Solver* theSolver);
    void removeSolver(Solver* theSolver);

    void resolveConstraints(float deltaTime);

    bool testCollider(const Collider& collider) const;
    bool testObject(const ObjectGeneral& theObject) const;

protected:
    void solveManifolds(std::vector<Manifold>& manifold, float deltaTime);
};
