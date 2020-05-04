#pragma once

#include <string>
#include <vector>

#include <deque>

#include <glm/glm.hpp>

#include "Types.h"
#include "Mesh.h"

struct Material
{
	Material()
	{
		name;
		specularExponent = 0.0f;
	}

	// Material Name
	std::string name;
	// Ambient Color
	glm::vec3 colAmbient;
	// Diffuse Color
	glm::vec3 colDiffuse;
	// Specular Color
	glm::vec3 colSpecular;
	// Specular Exponent
	float specularExponent;
	// Ambient Texture Map
	std::string mapAmbient;
	// Diffuse Texture Map
	std::string mapDiffuse;
	// Specular Texture Map
	std::string mapSpecular;
};

struct Vertex;

class ModelLoader {
public:
	// load new file, replacing all fields
	bool loadFile(std::string filePath);

	std::deque<Mesh> loadedMeshes;
	std::vector<Material> loadedMaterials;

private:
	// Generate vertices from a list of positions, tcoords, normals and a face line
	void verticesFromFaceString(std::vector<Vertex>& outputVertices, const std::vector<glm::vec3>& _positions, const std::vector<glm::vec2>& _texCoords, const std::vector<glm::vec3>& _normals, std::string _curLine);

	// Triangulate a list of vertices into a face by printing
	// indicies corresponding with triangles within it
	void verticesToIndicesTriangulated(std::vector<unsigned int>& indicesOut, const std::vector<Vertex>& _vertices);
};