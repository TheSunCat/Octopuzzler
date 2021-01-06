#include "Core/Rendering/MeshMarchedCube.h"


#include "Constants.h"
#include "Util.h"
#include "Core/Rendering/MarchingCubesData.h"

MeshMarchedCube::MeshMarchedCube(float* point0, float* point1, float* point2, float* point3, float* point4, float* point5, float* point6, float* point7, float surfaceLevel)
{
    points[0] = point0;
    points[1] = point1;
    points[2] = point2;
    points[3] = point3;
    points[4] = point4;
    points[5] = point5;
    points[6] = point6;
    points[7] = point7;
}

void MeshMarchedCube::update(float surfaceLevel)
{
    std::vector<Vertex> verts;

    int cubeIndex = 0;
    for(int i = 0; i < 8; i++)
    {
        if(*points[i] < surfaceLevel)
        {
            cubeIndex |= 1 << i;
        }
    }

    const int* triangulation = MarchingCubes::triangulation[cubeIndex];

    for (int i = 0; i < 16; i++)
    {
        int edgeIndex = triangulation[i];

        if (edgeIndex == -1)
            continue;

        // look up indices of the corner points making the current edge
        int indexA = MarchingCubes::cornerIndexAFromEdge[edgeIndex];
        int indexB = MarchingCubes::cornerIndexBFromEdge[edgeIndex];

        // find midpoint of edge
        verts.emplace_back(Util::lerp(cubeVerts[indexA].pos, cubeVerts[indexB].pos, 0.5f));
    }



    // dirty hack
    if(verts.empty())
    {
        hidden = true;
        return; // no need to update data, just skip to hide
    }

    hidden = false;

    //glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

    //std::vector<GLuint> indices;
    //for(int i = 0; i < verts.size(); i++)
    //{
    //    indices.emplace_back(i);
    //}

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}