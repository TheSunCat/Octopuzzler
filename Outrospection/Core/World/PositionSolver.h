#pragma once

#include "Solver.h"

class PositionSolver : public Solver
{
public:
    void solve(std::vector<Manifold>& manifolds, float deltaTime) override;
};