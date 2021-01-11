#include "PhysicsWorld.h"

#include "Core.h"

#include "Collider.h"
#include "Manifold.h"
#include "Rigidbody.h"
#include "Solver.h"

struct Manifold;
class Solver;

void PhysicsWorld::clearForces()
{
    for(auto* object : mObjects)
    {
        if (!object->isRigidbody) return;

        auto* rigidbody = (Rigidbody*)object;

        if (!rigidbody->isStatic)
            rigidbody->force = glm::vec3(0);
    }
}


PhysicsWorld::PhysicsWorld()
{
    mObjects = std::vector<ObjectGeneral*>();

    mObjects.push_back(new ObjectGeneral(glm::vec3(0), glm::vec3(0), glm::vec3(0)));

}

void PhysicsWorld::applyGravity()
{
    for (auto* object : mObjects)
    {
        if (!object->isRigidbody) return;

        auto* rigidbody = (Rigidbody*)object;

        if (!rigidbody->isStatic)
            rigidbody->applyGravity();
    }
}

void PhysicsWorld::predictTransforms(float deltaTime)
{
    float deltaTimeSq = deltaTime * deltaTime;

    for (auto* object : mObjects)
    {
        if (!object->isRigidbody) return;

        auto* rigidbody = (Rigidbody*)object;

        if (!rigidbody->isStatic)
        {
            Transform t = rigidbody->transform;
            glm::vec3 v = rigidbody->velocity;

            t.pos() += deltaTime * v + deltaTimeSq * rigidbody->force * rigidbody->invMass();

            rigidbody->velocity = v;
            rigidbody->setLastTransformation(t);
        }
    }
}


void PhysicsWorld::step(float deltaTime)
{
    applyGravity();

    predictTransforms(deltaTime);

    resolveConstraints(deltaTime);

    for(auto* object : mObjects)
    {
        if (!object->isRigidbody) return;

        auto* rigidbody = (Rigidbody*) object;

        rigidbody->setLastTransformation(rigidbody->transform);

        if(!rigidbody->isStatic)
        {
            rigidbody->velocity = deltaTime * rigidbody->force * rigidbody->invMass() + rigidbody->velocity;
            rigidbody->transform.pos() += deltaTime * rigidbody->velocity;
        }
    }

    clearForces();
}

void PhysicsWorld::addCollisionObject(
    ObjectGeneral* theObject)
{
    if (theObject == nullptr) {
        LOG_ERROR("Tried to add null object to collision space");
        return;
    }
    
    mObjects.push_back(theObject);
}

void PhysicsWorld::removeCollisionObject(
    ObjectGeneral* theObject)
{
    if (theObject == nullptr) {
        LOG_ERROR("Tried to remove null object from collision space");
        return;
    }

    auto itr = std::ranges::find(mObjects, theObject);

    if (itr == mObjects.end()) {
        LOG_ERROR("Tried to remove object that doesn't exist in the collision space");
        return;
    }

    mObjects.erase(itr);
}

void PhysicsWorld::addSolver(
    Solver* theSolver)
{
    if (theSolver == nullptr) {
        LOG_ERROR("Tried to add null solver to collision space");
        return;
    }

    mSolvers.push_back(theSolver);
}

void PhysicsWorld::removeSolver(
    Solver* theSolver)
{
    if (theSolver == nullptr) {
        LOG_ERROR("Tried to remove null solver from dynamics space");
        return;
    }

    auto itr = std::ranges::find(mSolvers, theSolver);

    if (itr == mSolvers.end()) {
        LOG_ERROR("Tried to remove solver that doesn't exist in the collision space");
        return;
    }

    mSolvers.erase(itr);
}

void PhysicsWorld::resolveConstraints(
    float deltaTime)
{
    std::vector<Manifold> manifolds;
    std::vector<Manifold> triggers;
    for (ObjectGeneral* a : mObjects) {
        for (ObjectGeneral* b : mObjects) {
            if (a == b) break;

            /*if (a->isTrigger()
            //	&& b->IsTrigger()
            //	&& !(a->IsDynamic() || b->IsDynamic()))
            //{
            //	continue;
            //}*/

            if (!a->collider
                || !b->collider)
            {
                continue;
            }

            ManifoldPoints points = a->collider->testCollision(&a->transform, b->collider, &b->transform);
            if (points.hasCollision) {
                /*// establish more formal rules for what can collide with what
                //if (a->IsTrigger()
                //	|| b->IsTrigger())
                //{
                //	triggers.emplace_back(a, b, points);
                //}

                //else {
                    manifolds.emplace_back(a, b, points);
                //}*/
            }
        }
    }

    //SendCollisionCallbacks(manifolds, dt);
    //SendCollisionCallbacks(triggers, dt);

    std::vector<Manifold> actuallyCollided;
    for (Manifold& manifold : manifolds) {
        if (manifold.hasCollision) {
            actuallyCollided.push_back(manifold); // big copy for no reason :(
        }
    }

    solveManifolds(actuallyCollided, deltaTime);
}

void PhysicsWorld::solveManifolds(std::vector<Manifold>& manifold, float deltaTime)
{
    for (Solver* solver : mSolvers)
    {
        solver->solve(manifold, deltaTime);
    }
}