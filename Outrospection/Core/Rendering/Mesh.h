#pragma once

#include <string>
#include <vector>

#include "Types.h"

#include "Core/Rendering/SimpleTexture.h"

class Shader;

class Mesh {
public:
	std::string name;
	SimpleTexture texture;

	Mesh() = default;

	// create a mesh with no texture
	Mesh(const std::string& _name, const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices);

	Mesh(const std::string& _name, const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices, const SimpleTexture& _texture);

	// render the mesh
	void draw(const Shader& shader) const;

private:
	unsigned int indicesSize;
	unsigned int VBO, VAO, EBO;
};