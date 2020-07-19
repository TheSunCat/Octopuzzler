#include "Mesh.h"

#include <GLAD/glad.h>

#include "Outrospection.h"
#include "Core/Rendering/TextureManager.h"

Mesh::Mesh(const std::string& _name, const std::vector<Vertex>& _vertices,
           const std::vector<GLuint>& _indices)
    : Mesh(_name, _vertices, _indices, TextureManager::missingTexture)
{
    
}

Mesh::Mesh(const std::string& _name, const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices,
           const SimpleTexture& _texture)
{
    name = _name;
    indicesSize = static_cast<unsigned int>(_indices.size());
    texture = _texture;

    // create buffers and arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), &_indices[0], GL_STATIC_DRAW);

    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    // norms
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    // tex coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    // tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
    // bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

    glBindVertexArray(0);
}

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