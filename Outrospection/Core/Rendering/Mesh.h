#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "SimpleTexture.h"

#include "../../Types.h"

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