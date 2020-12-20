#include "MeshCube.h"

static const std::vector<Vertex> cubeVerts = {
    // positions          
   {{ 0.5, 0.5, 0.5}}, // 000 1
   {{ 0.5, 0.5,-0.5}}, // 001 2
   {{ 0.5,-0.5, 0.5}}, // 010 3
   {{ 0.5,-0.5,-0.5}}, // 011 4
   {{-0.5,-0.5, 0.5}}, // 110 5
   {{-0.5,-0.5,-0.5}}, // 111 6
   {{-0.5, 0.5, 0.5}}, // 100 7
   {{-0.5, 0.5,-0.5}}, // 101 8
};

static const std::vector<GLuint> cubeIndices = { // TODO take credit
    0, 1, 3,
    0, 2, 3,
    2, 3, 5,
    2, 4, 5,
    4, 6, 7,
    4, 5, 7,
    6, 7, 1,
    6, 0, 1,
    2, 4, 6,
    2, 0, 6,
    5, 3, 1,
    5, 7, 1,
};

MeshCube::MeshCube() : Mesh(cubeVAO == 0 ? Mesh("cube", cubeVerts, cubeIndices) : Mesh("cube", cubeVAO))
{
    
}