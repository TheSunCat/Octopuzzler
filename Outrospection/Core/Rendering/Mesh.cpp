#include "Mesh.h"

#include <GLAD/glad.h>

#include <utility>

#include "Outrospection.h"
#include "Core/Rendering/TextureManager.h"

Mesh::Mesh(const std::string& _name, const std::vector<Vertex>& _vertices,
           const std::vector<GLuint>& _indices)
    : Mesh(_name, _vertices, _indices, TextureManager::missingTexture)
{
}

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
    glActiveTexture(GL_TEXTURE0);
    texture.bind();

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr);

    // reset vals
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}
