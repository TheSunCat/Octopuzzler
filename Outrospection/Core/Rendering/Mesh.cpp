#include "Mesh.h"

#include <GLAD/glad.h>

#include <utility>

#include "Outrospection.h"

Mesh::Mesh(const std::string& _name, const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices,
           const SimpleTexture& _texture) : Mesh(_name, &_vertices[0], _vertices.size(), &_indices[0], _indices.size(), _texture)
{
    
}

Mesh::Mesh(const std::string& _name, const Vertex* _vertices, unsigned int vertSize, const GLuint* _indices,
           unsigned int indSize, const SimpleTexture& _texture)
{
    name = _name;
    indicesSize = unsigned int(indSize);
    texture = _texture;

    // calculate bounding box
    auto min = glm::vec3(-INFINITY);
    auto max = glm::vec3( INFINITY);
    
    for(unsigned int i = 0; i < vertSize; i++)
    {
        const Vertex& v = _vertices[i];

        if (v.pos.x < bbox.min.x)
            bbox.min.x = v.pos.x;
        if (v.pos.y < bbox.min.y)
            bbox.min.y = v.pos.y;
        if (v.pos.z < bbox.min.z)
            bbox.min.z = v.pos.z;
        if (v.pos.x > bbox.max.x)
            bbox.max.x = v.pos.x;
        if (v.pos.y < bbox.max.y)
            bbox.max.y = v.pos.y;
        if (v.pos.z < bbox.max.z)
            bbox.max.z = v.pos.z;
    }

    bbox = AABB(min, max);

    // create buffers and arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize * sizeof(GLuint), &_indices[0], GL_STATIC_DRAW);

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

Mesh::Mesh(std::string _name, GLuint _VAO) : name(std::move(_name)), VAO(_VAO) {}

// render the mesh
void Mesh::draw() const
{
    if (hidden)
        return;

    glActiveTexture(GL_TEXTURE0);
    texture.bind();

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr);

    // reset vals
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}
