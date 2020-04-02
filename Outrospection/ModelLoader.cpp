#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include "util.h"

struct Vertex
{
	// Position Vector
	glm::vec3 pos;

	// Normal Vector
	glm::vec3 normal;

	// Texture Coordinate Vector
	glm::vec2 texCoord;
};

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

struct Mesh
{
	Mesh() = default;

	Mesh(std::vector<Vertex>& _vertices, std::vector<unsigned int>& _indices)
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

namespace algorithm
{
	// A test to see if P1 is on the same side as P2 of a line segment ab
	bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b)
	{
		glm::vec3 cp1 = glm::cross(b - a, p1 - a);
		glm::vec3 cp2 = glm::cross(b - a, p2 - a);

		if (glm::dot(cp1, cp2) >= 0)
			return true;
		else
			return false;
	}

	// Generate a cross produect normal for a triangle
	glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
	{
		glm::vec3 u = t2 - t1;
		glm::vec3 v = t3 - t1;

		glm::vec3 normal = glm::cross(u, v);

		return normal;
	}

	// Check to see if a glm::vec3 Point is within a 3 glm::vec3 Triangle
	bool inTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
	{
		// Test to see if it is within an infinite prism that the triangle outlines.
		bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
			&& SameSide(point, tri3, tri1, tri2);

		// If it isn't it will never be on the triangle
		if (!within_tri_prisim)
			return false;

		// Calulate Triangle's Normal
		glm::vec3 n = GenTriNormal(tri1, tri2, tri3);

		// Project the point onto this normal
		glm::vec3 proj = projectV3(point, n);

		// If the distance from the triangle to the point is 0
		//	it lies on the triangle
		if (glm::length(proj) == 0)
			return true;
		else
			return false;
	}

	// Split a String into a string array at a given token
	inline void split(const std::string& in, std::vector<std::string>& out, std::string token)
	{
		out.clear();

		std::string temp;

		for (int i = 0; i < in.size(); i++)
		{
			std::string test = in.substr(i, token.size());

			if (test == token)
			{
				if (!temp.empty())
				{
					out.push_back(temp);
					temp.clear();
					i += (int)token.size() - 1;
				}
				else
				{
					out.push_back("");
				}
			}
			else if (i + token.size() >= in.size())
			{
				temp += in.substr(i, token.size());
				out.push_back(temp);
				break;
			}
			else
			{
				temp += in[i];
			}
		}
	}

	// Get tail of string after first token and possibly following spaces
	inline std::string tail(const std::string& in)
	{
		unsigned int token_start = in.find_first_not_of(" \t");
		unsigned int space_start = in.find_first_of(" \t", token_start);
		unsigned int tail_start = in.find_first_not_of(" \t", space_start);
		unsigned int tail_end = in.find_last_not_of(" \t");

		if (tail_start != std::string::npos) {
			if (tail_end != std::string::npos)
				return in.substr(tail_start, tail_end - tail_start + 1);
			else
				return in.substr(tail_start);
		}

		return "";
	}

	// Get first token of string
	inline std::string firstToken(const std::string& in)
	{
		if (!in.empty())
		{
			unsigned int token_start = in.find_first_not_of(" \t");
			unsigned int token_end = in.find_first_of(" \t", token_start);
			if (token_start != std::string::npos && token_end != std::string::npos)
			{
				return in.substr(token_start, token_end - token_start);
			}
			else if (token_start != std::string::npos)
			{
				return in.substr(token_start);
			}
		}
		return "";
	}

	// Get element at given index position
	template <class T>
	inline const T& getElement(const std::vector<T>& elements, std::string& indexStr)
	{
		int index = std::stoi(indexStr);

		if (index < 0)
			index = elements.size() + index;
		else
			index--;

		return elements[index];
	}
}

class ModelLoader {
public:
	bool loadFile(std::string filePath)
	{
		// If the file is not an .obj file return false
		if (filePath.substr(filePath.size() - 4, 4) != ".obj")
			return false;


		std::ifstream file(filePath);

		if (!file.is_open())
			return false;

		loadedMeshes.clear();
		loadedVertices.clear();
		loadedIndices.clear();

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		std::vector<std::string> meshMatNames;

		// controls whether the mesh has been named or not. name should be first 'o' or 'g'
		bool listening = false;
		std::string meshName;

		Mesh tempMesh;

		std::string curLine;
		while (std::getline(file, curLine)) {
			if (curLine == "")
				continue;

			// Generate a Mesh Object or Prepare for an object to be created
			if (algorithm::firstToken(curLine) == "o" || algorithm::firstToken(curLine) == "g" || curLine[0] == 'g')
			{
				if (!listening)
				{
					listening = true;

					// name the mesh
					if (algorithm::firstToken(curLine) == "o" || algorithm::firstToken(curLine) == "g")
					{
						meshName = algorithm::tail(curLine);
					}
					else
					{
						meshName = "unnamed";
					}
				}
				else
				{
					// Generate the mesh to put into the array
					if (!indices.empty() && !vertices.empty()) // if there are verts and indices
					{
						// Create Mesh
						tempMesh = Mesh(vertices, indices);
						tempMesh.meshName = meshName;

						// Insert Mesh
						loadedMeshes.push_back(tempMesh);

						// Cleanup
						vertices.clear();
						indices.clear();
						meshName.clear();

						meshName = algorithm::tail(curLine);
					}
					else
					{
						// name the mesh
						if (algorithm::firstToken(curLine) == "o" || algorithm::firstToken(curLine) == "g")
						{
							meshName = algorithm::tail(curLine);
						}
						else
						{
							meshName = "unnamed";
						}
					}
				}
			}

			// Generate a Vertex Position
			if (algorithm::firstToken(curLine) == "v")
			{
				std::vector<std::string> vertPosStr;
				glm::vec3 vertPos;

				algorithm::split(algorithm::tail(curLine), vertPosStr, " ");

				vertPos.x = std::stof(vertPosStr[0]);
				vertPos.y = std::stof(vertPosStr[1]);
				vertPos.z = std::stof(vertPosStr[2]);

				positions.push_back(vertPos);
			}

			// Generate a Vertex Texture Coordinate
			if (algorithm::firstToken(curLine) == "vt")
			{
				std::vector<std::string> texCoordStr;
				glm::vec2 texCoord;

				algorithm::split(algorithm::tail(curLine), texCoordStr, " ");

				texCoord.x = std::stof(texCoordStr[0]);
				texCoord.y = std::stof(texCoordStr[1]);

				texCoords.push_back(texCoord);
			}

			// Generate a Vertex Normal;
			if (algorithm::firstToken(curLine) == "vn")
			{
				std::vector<std::string> vertNormStr;
				glm::vec3 vertNorm;

				algorithm::split(algorithm::tail(curLine), vertNormStr, " ");

				vertNorm.x = std::stof(vertNormStr[0]);
				vertNorm.y = std::stof(vertNormStr[1]);
				vertNorm.z = std::stof(vertNormStr[2]);

				normals.push_back(vertNorm);
			}

			// Generate a Face (vertices & indices)
			if (algorithm::firstToken(curLine) == "f")
			{
				// Generate the vertices
				std::vector<Vertex> loadedVerticesVector;
				verticesFromFaceString(loadedVerticesVector, positions, texCoords, normals, curLine);

				// Add Vertices
				for (int i = 0; i < loadedVerticesVector.size(); i++)
				{
					Vertex curVert = loadedVerticesVector[i];

					vertices.push_back(curVert);

					loadedVertices.push_back(curVert);
				}

				std::vector<unsigned int> indicesVector;

				verticesToIndicesTriangulated(indicesVector, loadedVerticesVector);

				// Add Indices
				for (int i = 0; i < indicesVector.size(); i++)
				{
					unsigned int curIndex = (unsigned int) ((vertices.size()) - loadedVerticesVector.size()) + indicesVector[i];
					indices.push_back(curIndex);

					curIndex = (unsigned int) ((loadedVertices.size()) - loadedVerticesVector.size()) + indicesVector[i];
					loadedIndices.push_back(curIndex);
				}
			}

			// Get Mesh Material Name
			if (algorithm::firstToken(curLine) == "usemtl")
			{
				meshMatNames.push_back(algorithm::tail(curLine));

				// Create new Mesh, if Material changes within a group
				if (!indices.empty() && !vertices.empty())
				{
					// Create Mesh
					tempMesh = Mesh(vertices, indices);
					tempMesh.meshName = meshName;

					// rename new mesh
					int i = 2;
					while (true) {
						tempMesh.meshName = meshName + "_" + std::to_string(i);

						// check for duplicate names
						for (Mesh& m : loadedMeshes)
							if (m.meshName == tempMesh.meshName)
								continue;

						break;
					}

					// insert mesh
					loadedMeshes.push_back(tempMesh);

					// cleanup
					vertices.clear();
					indices.clear();
				}
			}

			// Load Materials
			if (algorithm::firstToken(curLine) == "mtllib")
			{
				// Generate a path to the material file
				std::vector<std::string> splitFilePath;
				algorithm::split(filePath, splitFilePath, "/");

				std::string pathtomat = "";

				if (splitFilePath.size() != 1)
				{
					for (int i = 0; i < splitFilePath.size() - 1; i++)
					{
						pathtomat += splitFilePath[i] + "/";
					}
				}

				pathtomat += algorithm::tail(curLine);

				// Load Materials
				loadMats(pathtomat);
			}
		}

		// Deal with last mesh
		if (!indices.empty() && !vertices.empty())
		{
			// Create Mesh
			tempMesh = Mesh(vertices, indices);
			tempMesh.meshName = meshName;

			// Insert Mesh
			loadedMeshes.push_back(tempMesh);
		}

		file.close();

		// Set Materials for each Mesh
		for (int i = 0; i < meshMatNames.size(); i++)
		{
			std::string matname = meshMatNames[i];

			// Find corresponding material name in loaded materials
			// when found copy material variables into mesh material
			for (int j = 0; j < loadedMaterials.size(); j++)
			{
				if (loadedMaterials[j].name == matname)
				{
					loadedMeshes[i].meshMaterial = loadedMaterials[j];
					break;
				}
			}
		}

		if (loadedMeshes.empty() && loadedVertices.empty() && loadedIndices.empty())
			return false;
		else
			return true;
	}

	// Loaded Mesh Objects
	std::vector<Mesh> loadedMeshes;
	// Loaded Vertex Objects
	std::vector<Vertex> loadedVertices;
	// Loaded Index Positions
	std::vector<unsigned int> loadedIndices;
	// Loaded Material Objects
	std::vector<Material> loadedMaterials;

private:
	// Generate vertices from a list of positions, tcoords, normals and a face line
	void verticesFromFaceString(std::vector<Vertex>& outputVertices, const std::vector<glm::vec3>& _positions, const std::vector<glm::vec2>& _texCoords, const std::vector<glm::vec3>& _normals, std::string _curLine)
	{
		std::vector<std::string> splitFaceStr;
		Vertex vertex;
		algorithm::split(algorithm::tail(_curLine), splitFaceStr, " ");

		bool noNormal = false;

		// For every given vertex do this
		for (int i = 0; i < splitFaceStr.size(); i++)
		{
			std::vector<std::string> splitVertexStr;
			algorithm::split(splitFaceStr[i], splitVertexStr, "/");

			// 1 = pos, 2 = pos/tex, 3 = pos/norm,, 4 = pos/tex/norm
			int vertexType;

			// Check for just position - v1
			if (splitVertexStr.size() == 1)
			{
				// Only position
				vertexType = 1;
			}

			// Check for position & texture - v1/vt1
			if (splitVertexStr.size() == 2)
			{
				// Position & Texture
				vertexType = 2;
			}

			// Check for Position, Texture and Normal - v1/vt1/vn1
			// or if Position and Normal - v1//vn1
			if (splitVertexStr.size() == 3)
			{
				if (splitVertexStr[1] != "")
				{
					// Position, Texture, and Normal
					vertexType = 4;
				}
				else
				{
					// Position & Normal
					vertexType = 3;
				}
			}

			// Calculate and store the vertex
			switch (vertexType)
			{
			case 1: // P
			{
				vertex.pos = algorithm::getElement(_positions, splitVertexStr[0]);
				vertex.texCoord = glm::vec2(0, 0);
				noNormal = true;
				outputVertices.push_back(vertex);
				break;
			}
			case 2: // P/T
			{
				vertex.pos = algorithm::getElement(_positions, splitVertexStr[0]);
				vertex.texCoord = algorithm::getElement(_texCoords, splitVertexStr[1]);
				noNormal = true;
				outputVertices.push_back(vertex);
				break;
			}
			case 3: // P//N
			{
				vertex.pos = algorithm::getElement(_positions, splitVertexStr[0]);
				vertex.texCoord = glm::vec2(0, 0);
				vertex.normal = algorithm::getElement(_normals, splitVertexStr[2]);
				outputVertices.push_back(vertex);
				break;
			}
			case 4: // P/T/N
			{
				vertex.pos = algorithm::getElement(_positions, splitVertexStr[0]);
				vertex.texCoord = algorithm::getElement(_texCoords, splitVertexStr[1]);
				vertex.normal = algorithm::getElement(_normals, splitVertexStr[2]);
				outputVertices.push_back(vertex);
				break;
			}
			default:
			{
				break;
			}
			}
		}

		// take care of missing normals
		if (noNormal)
		{
			glm::vec3 A = outputVertices[0].pos - outputVertices[1].pos;
			glm::vec3 B = outputVertices[2].pos - outputVertices[1].pos;

			glm::vec3 normal = glm::cross(A, B);

			for (int i = 0; i < outputVertices.size(); i++)
			{
				outputVertices[i].normal = normal;
			}
		}
	}

	// Triangulate a list of vertices into a face by printing
	// indicies corresponding with triangles within it
	void verticesToIndicesTriangulated(std::vector<unsigned int>& indicesOut, const std::vector<Vertex>& _vertices)
	{
		// less than 3 verts means no tris, so exit
		if (_vertices.size() < 3)
			return;

		// it's just one tri, sweet
		if (_vertices.size() == 3)
		{
			indicesOut.push_back(0);
			indicesOut.push_back(1);
			indicesOut.push_back(2);
			return;
		}

		// Create a list of vertices
		std::vector<Vertex> verticesVector = _vertices;

		while (true)
		{
			// For every vertex
			for (int index = 0; index < verticesVector.size(); index++)
			{
				Vertex prevVertex;

				if (index == 0)
					prevVertex = verticesVector[verticesVector.size() - 1];
				else
					prevVertex = verticesVector[index - 1];

				Vertex curVertex = verticesVector[index];

				Vertex nextVertex;
				if (index == verticesVector.size() - 1)
					nextVertex = verticesVector[0];
				else
					nextVertex = verticesVector[index + 1];

				// 3 verts left means this is the last tri
				if (verticesVector.size() == 3)
				{
					// create tri from current, previous, and next vert
					for (int j = 0; j < verticesVector.size(); j++)
					{
						if (_vertices[j].pos == curVertex.pos)
							indicesOut.push_back(j);
						if (_vertices[j].pos == prevVertex.pos)
							indicesOut.push_back(j);
						if (_vertices[j].pos == nextVertex.pos)
							indicesOut.push_back(j);
					}

					// we're done!
					verticesVector.clear();
					break;
				}

				if (verticesVector.size() == 4)
				{
					// create tri from current, previous, and next vert
					for (int j = 0; j < _vertices.size(); j++)
					{
						if (_vertices[j].pos == curVertex.pos)
							indicesOut.push_back(j);
						if (_vertices[j].pos == prevVertex.pos)
							indicesOut.push_back(j);
						if (_vertices[j].pos == nextVertex.pos)
							indicesOut.push_back(j);
					}

					glm::vec3 tempVec;
					for (int j = 0; j < verticesVector.size(); j++)
					{
						if (verticesVector[j].pos != curVertex.pos
							&& verticesVector[j].pos != prevVertex.pos
							&& verticesVector[j].pos != nextVertex.pos)
						{
							tempVec = verticesVector[j].pos;
							break;
						}
					}

					// create tri from current, previous, and next vert
					for (int j = 0; j < _vertices.size(); j++)
					{
						if (_vertices[j].pos == prevVertex.pos)
							indicesOut.push_back(j);
						if (_vertices[j].pos == nextVertex.pos)
							indicesOut.push_back(j);
						if (_vertices[j].pos == tempVec)
							indicesOut.push_back(j);
					}

					// we're done!
					verticesVector.clear();
					break;
				}

				// If Vertex is not an interior vertex
				float angle = angleBetweenV3(prevVertex.pos - curVertex.pos, nextVertex.pos - curVertex.pos) * (180 / 3.14159265359);
				if (angle <= 0 && angle >= 180)
					continue;

				// If any vertices are within this triangle
				bool inTri = false;
				for (int j = 0; j < _vertices.size(); j++)
				{
					if (algorithm::inTriangle(_vertices[j].pos, prevVertex.pos, curVertex.pos, nextVertex.pos)
						&& _vertices[j].pos != prevVertex.pos
						&& _vertices[j].pos != curVertex.pos
						&& _vertices[j].pos != nextVertex.pos)
					{
						inTri = true;
						break;
					}
				}

				if (inTri)
					continue;

				// create tri from current, previous, and next vert
				for (int j = 0; j < _vertices.size(); j++)
				{
					if (_vertices[j].pos == curVertex.pos)
						indicesOut.push_back(j);
					if (_vertices[j].pos == prevVertex.pos)
						indicesOut.push_back(j);
					if (_vertices[j].pos == nextVertex.pos)
						indicesOut.push_back(j);
				}

				// remove curVertex from the working list
				for (int j = 0; j < verticesVector.size(); j++)
				{
					if (verticesVector[j].pos == curVertex.pos)
					{
						verticesVector.erase(verticesVector.begin() + j);
						break;
					}
				}

				// reset index to the start (-1 since loop will add 1 to it)
				index = -1;
			}

			// no triangles were created
			if (indicesOut.size() == 0)
				break;

			// no more vertices
			if (verticesVector.size() == 0)
				break;
		}
	}

	// load mats from .mtl file
	bool loadMats(std::string path)
	{
		// return false if file is not mtl
		if (path.substr(path.size() - 4, path.size()) != ".mtl")
			return false;

		std::ifstream mtlFile(path);

		// return false if file is not found
		if (!mtlFile.is_open())
			return false;

		Material tempMaterial;

		bool listening = false;

		// Go through each line looking for material variables
		std::string curline;
		while (std::getline(mtlFile, curline))
		{
			// new material and material name
			if (algorithm::firstToken(curline) == "newmtl")
			{
				if (!listening)
				{
					listening = true;

					// name the mat
					if (curline.size() > 7)
						tempMaterial.name = algorithm::tail(curline);
					else
						tempMaterial.name = "none";
				}
				else
				{
					// push the temp mat, to prepare for new mat
					loadedMaterials.push_back(tempMaterial);

					// reset temp mat
					tempMaterial = Material();

					// name the mat
					if (curline.size() > 7)
						tempMaterial.name = algorithm::tail(curline);
					else
						tempMaterial.name = "none";
				}
			}

			// Ambient Color
			if (algorithm::firstToken(curline) == "Ka")
			{
				std::vector<std::string> temp;
				algorithm::split(algorithm::tail(curline), temp, " ");

				if (temp.size() != 3)
					continue;

				tempMaterial.colAmbient.x = std::stof(temp[0]);
				tempMaterial.colAmbient.y = std::stof(temp[1]);
				tempMaterial.colAmbient.z = std::stof(temp[2]);
			}

			// Diffuse Color
			if (algorithm::firstToken(curline) == "Kd")
			{
				std::vector<std::string> temp;
				algorithm::split(algorithm::tail(curline), temp, " ");

				if (temp.size() != 3)
					continue;

				tempMaterial.colDiffuse.x = std::stof(temp[0]);
				tempMaterial.colDiffuse.y = std::stof(temp[1]);
				tempMaterial.colDiffuse.z = std::stof(temp[2]);
			}

			// Specular Color
			if (algorithm::firstToken(curline) == "Ks")
			{
				std::vector<std::string> temp;
				algorithm::split(algorithm::tail(curline), temp, " ");

				if (temp.size() != 3)
					continue;

				tempMaterial.colSpecular.x = std::stof(temp[0]);
				tempMaterial.colSpecular.y = std::stof(temp[1]);
				tempMaterial.colSpecular.z = std::stof(temp[2]);
			}

			// Specular Exponent
			if (algorithm::firstToken(curline) == "Ns")
			{
				tempMaterial.specularExponent = std::stof(algorithm::tail(curline));
			}

			// Ambient Texture Map
			if (algorithm::firstToken(curline) == "map_Ka")
			{
				tempMaterial.mapAmbient = algorithm::tail(curline);
			}

			// Diffuse Texture Map
			if (algorithm::firstToken(curline) == "map_Kd")
			{
				tempMaterial.mapDiffuse = algorithm::tail(curline);
			}

			// Specular Texture Map
			if (algorithm::firstToken(curline) == "map_Ks")
			{
				tempMaterial.mapSpecular = algorithm::tail(curline);
			}
		}

		// take care of the last material
		loadedMaterials.push_back(tempMaterial);

		// return whether something was loaded
		return !(loadedMaterials.empty());
	}
};