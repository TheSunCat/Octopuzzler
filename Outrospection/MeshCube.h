#pragma once
#include "Core/Rendering/Mesh.h"

class MeshCube : public Mesh
{
public:
    MeshCube();

    static inline GLuint cubeVAO = 0;
};
