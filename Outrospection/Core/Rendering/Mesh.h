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
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	SimpleTexture texture;
	unsigned int VAO;

	Mesh() = default;

	Mesh(std::string _name, std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, SimpleTexture _texture);

	// render the mesh
	void draw(const Shader& shader);

private:
	unsigned int VBO, EBO;

	void setupMesh();
};