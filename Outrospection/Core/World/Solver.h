#pragma once
#include <vector>

struct Manifold;

class Solver
{
public:
    virtual void solve(std::vector<Manifold>& manifolds, float deltaTime) = 0;
};
