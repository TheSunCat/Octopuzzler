#pragma once
#include "Outrospection.h"
#include "Util.h"
#include "Core/Rendering/Mesh.h"
#include "Core/Rendering/TextureManager.h"

class MeshSphere : public Mesh
{
public:
    MeshSphere(unsigned int levels = 1);

    const std::vector<int>& indices(int level) const;
    void draw(int level);
protected:
    void subdivide();
    std::vector<Vertex> mVertices;
    std::vector<std::vector<int>*> mIndices;
    std::vector<int> mListIds;


//public:
//    MeshSphere(float radius, int subdivisions)
//    {
//        radius = 1;
//
//        std::vector<glm::vec3> vertices;
//        std::vector<GLuint> indices;
//
//        for(auto& i : vdata)
//            vertices.emplace_back(i * radius);
//
//        for (auto& i : triangleIndices)
//        {
//            indices.emplace_back(i[0]);
//            indices.emplace_back(i[1]);
//            indices.emplace_back(i[2]);
//        }
//
//        for (int i = 0; i < subdivisions; ++i)
//        {
//            indices = Util::subdivide(vertices, indices);
//        }
//
//
//
//
//        name = "sphere";
//        indicesSize = static_cast<unsigned int>(indices.size());
//
//        Resource res = Resource("Textures/", "testSS00.png");
//        texture = Outrospection::get().textureManager.get(res);
//
//
//
//
//        // create buffers and arrays
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glGenBuffers(1, &EBO);
//
//        glBindVertexArray(VAO);
//
//        // load data into vertex buffers
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
//
//        // pos
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
//        // norms
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
//        // tex coords
//        glEnableVertexAttribArray(2);
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//            reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
//        // tangents
//        glEnableVertexAttribArray(3);
//        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
//        // bitangents
//        glEnableVertexAttribArray(4);
//        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//            reinterpret_cast<void*>(offsetof(Vertex, bitangent)));
//
//        glBindVertexArray(0);
//    }
};
