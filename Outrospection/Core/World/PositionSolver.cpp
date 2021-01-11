#include "PositionSolver.h"

#include "Manifold.h"
#include "ObjectGeneral.h"

void PositionSolver::solve(std::vector<Manifold>& manifolds, float deltaTime)
{
    for (Manifold& manifold : manifolds)
    {
        ObjectGeneral* aBody = manifold.objA;
        ObjectGeneral* bBody = manifold.objB;

        glm::vec3 resolution = manifold.n * manifold.penetrationDepth;

        aBody->transform.pos() -= resolution * (1 - float(aBody->isStatic));
        bBody->transform.pos() += resolution * (1 - float(aBody->isStatic));
    }
}
