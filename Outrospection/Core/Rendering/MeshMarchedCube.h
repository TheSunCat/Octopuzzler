#pragma once
#include <array>

#include "Core/Rendering/Mesh.h"

class MeshMarchedCube : public Mesh
{
public:
    MeshMarchedCube(float* point0, float* point1, float* point2, float* point3, float* point4, float* point5, float* point6, float* point7, float surfaceLevel = 0.4f);

    void update(float surfaceLevel);

private:
    std::array<float*, 8> points{};
};
