#pragma once
#include "Core/Rendering/Mesh.h"

class MeshCube : public Mesh
{
public:
    MeshCube();

    unsigned int color = 0;

    static inline GLuint cubeVAO = 0;
};
