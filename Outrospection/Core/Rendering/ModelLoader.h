#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../../Types.h"

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

struct MeshData
{
	MeshData() = default;

	MeshData(std::vector<Vertex>& _vertices, std::vector<unsigned int>& _indices)
	{
		vertices = _vertices;
		indices = _indices;
	}

	// Mesh Name
	std::string meshName;
	// Vertex List
	std::vector<Vertex> vertices;
	// Index List
	std::vector<unsigned int> indices;

	// Material
	Material meshMaterial;
};

struct Vertex;

class ModelLoader {
public:
	bool loadFile(std::string filePath);

	// Loaded Mesh Objects
	std::vector<MeshData> loadedMeshes;
	// Loaded Vertex Objects
	std::vector<Vertex> loadedVertices;
	// Loaded Index Positions
	std::vector<unsigned int> loadedIndices;
	// Loaded Material Objects
	std::vector<Material> loadedMaterials;

private:
	// Generate vertices from a list of positions, tcoords, normals and a face line
	void verticesFromFaceString(std::vector<Vertex>& outputVertices, const std::vector<glm::vec3>& _positions, const std::vector<glm::vec2>& _texCoords, const std::vector<glm::vec3>& _normals, std::string _curLine);

	// Triangulate a list of vertices into a face by printing
	// indicies corresponding with triangles within it
	void verticesToIndicesTriangulated(std::vector<unsigned int>& indicesOut, const std::vector<Vertex>& _vertices);

	// load mats from .mtl file
	bool loadMats(std::string path);
};