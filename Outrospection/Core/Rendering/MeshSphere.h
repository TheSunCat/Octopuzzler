#pragma once
#include "Outrospection.h"
#include "Core/Rendering/Mesh.h"

class MeshSphere : public Mesh
{
public:
    MeshSphere(unsigned int levels = 1);

    const std::vector<int>& indices(int level) const;
    void draw(int level);

    std::vector<Vertex> mVertices;
    std::vector<std::vector<int>*> mIndices;

protected:
    void subdivide();
    std::vector<int> mListIds;
};