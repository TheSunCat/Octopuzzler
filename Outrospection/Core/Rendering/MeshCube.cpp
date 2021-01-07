#include "MeshCube.h"

#include "Constants.h"

MeshCube::MeshCube() : Mesh(cubeVAO == 0
                                ? Mesh("cube", &cubeVerts[0], cubeVerts.size(), &cubeIndices[0], cubeIndices.size(),
                                       TextureManager::missingTexture)
                                : Mesh("cube", cubeVAO))
{
    
}
