#pragma once
#include "Outrospection.h"
#include "Util.h"
#include "Core/Rendering/Mesh.h"
#include "Core/Rendering/TextureManager.h"

class MeshSphere : public Mesh
{

#define X .525731112119133606 
#define Z .850650808352039932
inline static glm::vec3 vdata[12] = {
   glm::vec3(-X, 0.0, Z), glm::vec3(X, 0.0, Z), glm::vec3(-X, 0.0, -Z), glm::vec3(X, 0.0, -Z),
   glm::vec3(0.0, Z, X), glm::vec3(0.0, Z, -X), glm::vec3(0.0, -Z, X), glm::vec3(0.0, -Z, -X),
   glm::vec3(Z, X, 0.0), glm::vec3(-Z, X, 0.0), glm::vec3(Z, -X, 0.0), glm::vec3(-Z, -X, 0.0)
};

inline static GLuint tindices[20][3] = {
    {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
    {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

public:
    MeshSphere(float radius, int resolution)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        for (int i = 0; i < 20; i++) {
            Util::subdivide(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], resolution, vertices);
        }

        for (int i = 0; i < vertices.size(); i++)
        {
            indices.push_back(i);

            vertices[i].pos *= radius;
            vertices[i].texCoord = glm::vec2(i % 3);
        }

        name = "sphere";
        indicesSize = static_cast<unsigned int>(indices.size());

        Resource res = Resource("Textures/", "testSS00.png");
        texture = Outrospection::get().textureManager.get(res);

        // create buffers and arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        // pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        // norms
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        // tex coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
        // tangents
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
        // bitangents
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

        glBindVertexArray(0);
    }
};
