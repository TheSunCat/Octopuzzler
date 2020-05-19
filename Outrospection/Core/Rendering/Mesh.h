#pragma once

#include <string>
#include <vector>

#include <GLAD/glad.h>

#include "Types.h"

#include "Core/Rendering/SimpleTexture.h"

class Shader;

class Mesh {
public:
	std::string name;
	SimpleTexture texture;

	Mesh() = default;

	// create a mesh with no texture
	Mesh(const std::string& _name, const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices);

	Mesh(const std::string& _name, const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices,
	     const SimpleTexture& _texture);

	// render the mesh
	void draw() const;

private:
	unsigned int indicesSize = 0;
	GLuint VBO = 0, VAO = 0, EBO = 0;
};