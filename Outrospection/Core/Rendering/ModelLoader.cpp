#include "ModelLoader.h"

#include <fstream>

#include "TextureManager.h"
#include "../../Util.h"
#include "../../Source.h"

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
		glm::vec3 proj = Util::projectV3(point, n);

		// If the distance from the triangle to the point is 0
		//	it lies on the triangle
		if (glm::length(proj) == 0)
			return true;
		else
			return false;
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

bool ModelLoader::loadFile(std::string filePath)
{
	// if the file is not an .obj file return false
	if (filePath.substr(filePath.size() - 4, 4) != ".omd")
		return false;


	std::ifstream file(filePath);

	if (!file.is_open())
		return false;

	loadedMeshes.clear();

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::vector<std::string> meshMatNames;

	// controls whether the mesh has been named or not. name should be first 'o' or 'g'
	bool listening = false;
	std::string meshName;

	//MeshData tempMesh;

	bool matListening = false;
	Material tempMaterial;

	std::string curLine;
	while (std::getline(file, curLine)) {
		if (curLine == "" || curLine[0] == '#')
			continue;

		
		switch (curLine[0]) {
		case 'o': // Generate a Mesh Object or Prepare for an object to be created
		{
			if (!listening)
			{
				listening = true;

				// name the mesh
				meshName = curLine.substr(2);
			}
			else // if(listening)
			{
				// Generate the mesh to put into the array
				if (!indices.empty() && !vertices.empty()) // if there are verts and indices
				{
					// ceate and insert Mesh
					loadedMeshes.emplace_back(meshName, vertices, indices);

					// Cleanup
					vertices.clear();
					indices.clear();
					meshName.clear();

					meshName = curLine.substr(2);
				}
				else
				{
					// name the mesh
					meshName = curLine.substr(2);
				}
			}
			break;
		}
		case 'v': // Generate a Vertex Position
		{
			std::vector<std::string> vertPosStr;
			Util::split(curLine.substr(2), ' ', vertPosStr);

			positions.emplace_back(std::stof(vertPosStr[0]), std::stof(vertPosStr[1]), std::stof(vertPosStr[2]));

			vertPosStr.clear();
			break;
		}
		
		case 't': // Generate a Vertex Texture Coordinate
		{
			std::vector<std::string> texCoordStr;
			Util::split(curLine.substr(2), ' ', texCoordStr);

			texCoords.emplace_back(std::stof(texCoordStr[0]), std::stof(texCoordStr[1]));

			texCoordStr.clear();

			break;
		}
		// Generate a Vertex Normal;
		case 'n':
		{
			std::vector<std::string> vertNormStr;
			Util::split(curLine.substr(2), ' ', vertNormStr);

			normals.emplace_back(std::stof(vertNormStr[0]), std::stof(vertNormStr[1]), std::stof(vertNormStr[2]));
			vertNormStr.clear();
			break;
		}
		// Generate a Face (vertices & indices)
		case 'f':
		{
			// Generate the vertices
			std::vector<Vertex> vertsFromFace;
			verticesFromFaceString(vertsFromFace, positions, texCoords, normals, curLine);

			// Add Vertices
			for (int i = 0; i < vertsFromFace.size(); i++)
			{
				vertices.emplace_back(vertsFromFace[i]);
			}

			std::vector<unsigned int> indicesVector;

			verticesToIndicesTriangulated(indicesVector, vertsFromFace);

			// Add Indices
			for (int i = 0; i < indicesVector.size(); i++)
			{
				unsigned int curIndex = (unsigned int)((vertices.size()) - vertsFromFace.size()) + indicesVector[i];
				indices.push_back(curIndex);
			}

			vertsFromFace.clear();
			break;
		}

		// MATERIAL
		// --------
		case 'm':
		{
			switch (curLine[1]) {
			case 'u': // usemtl
			{
				meshMatNames.emplace_back(curLine.substr(3));

				// Create new Mesh, if Material changes within a group
				if (!indices.empty() && !vertices.empty())
				{
					// Create Mesh
					loadedMeshes.emplace_back(meshName, vertices, indices);

					// rename new mesh
					int i = 2;
					while (true) {
						loadedMeshes.back().name = meshName + "_" + std::to_string(i);

						// check for duplicate names
						for (const Mesh& m : loadedMeshes)
							if (m.name == loadedMeshes.back().name)
								continue;

						break;
					}

					// cleanup
					vertices.clear();
					indices.clear();
				}
				break;
			}

			case 'n': // new material and material name
			{
				if (!matListening)
				{
					matListening = true;

					// name the mat
					if (curLine.size() > 3)
						tempMaterial.name = curLine.substr(3);
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
					if (curLine.size() > 3)
						tempMaterial.name = curLine.substr(3);
					else
						tempMaterial.name = "none";
				}
				break;
			}
			}
			break;
		}

		case 'K':
		{
			switch (curLine[1]) {
			case 'd': // Kd, Diffuse Color
			{
				std::vector<std::string> temp;
				Util::split(curLine.substr(3), ' ', temp);

				if (temp.size() != 3)
					continue;

				tempMaterial.colDiffuse.x = std::stof(temp[0]);
				tempMaterial.colDiffuse.y = std::stof(temp[1]);
				tempMaterial.colDiffuse.z = std::stof(temp[2]);

				temp.clear();
				break;
			}

			// Specular Color
			case 's': // Ks, Specular Color
			{
				std::vector<std::string> temp;
				Util::split(curLine.substr(3), ' ', temp);

				if (temp.size() != 3)
					continue;

				tempMaterial.colSpecular.x = std::stof(temp[0]);
				tempMaterial.colSpecular.y = std::stof(temp[1]);
				tempMaterial.colSpecular.z = std::stof(temp[2]);

				temp.clear();
				break;
			}

			// Ambient Color
			case 'a':
			{
				std::vector<std::string> temp;
				Util::split(curLine.substr(3), ' ', temp);

				if (temp.size() != 3)
					continue;

				tempMaterial.colAmbient.x = std::stof(temp[0]);
				tempMaterial.colAmbient.y = std::stof(temp[1]);
				tempMaterial.colAmbient.z = std::stof(temp[2]);

				temp.clear();
				break;
			}
			}

			break;
		}

		// Specular Exponent
		case 's':
		{
			if (curLine[2] == 'o') // off
				tempMaterial.specularExponent = 0;
			else
				tempMaterial.specularExponent = std::stof(curLine.substr(1));
			break;
		}


		case 'M':
		{
			switch (curLine[1])
			{
				// Ambient Texture Map
			case 'a': // map_Ka
			{
				tempMaterial.mapAmbient = curLine.substr(3);
				break;
			}

			// Diffuse Texture Map
			case 'd':
			{
				tempMaterial.mapDiffuse = curLine.substr(3);
				break;
			}

			// Specular Texture Map
			case 's':
			{
				tempMaterial.mapSpecular = curLine.substr(3);
				break;
			}
			}
		}
		}
	}

	// Deal with last Mesh
	if (!vertices.empty() && !indices.empty())
	{
		// create and insert Mesh
		loadedMeshes.emplace_back(meshName, vertices, indices);

		vertices.clear();
		indices.clear();
	}

	// take care of the last material
	loadedMaterials.push_back(tempMaterial);

	file.close();

	TextureManager* _textureManager = &(getOutrospection()->textureManager);
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
				// TODO support more mat stuff (and write it to the Mesh obj)

				Resource r("Textures/", loadedMaterials[j].mapDiffuse);

				loadedMeshes[i].texture = _textureManager->loadTexture(r);
				break;
			}
		}
	}

	if (loadedMeshes.empty())
		return false;
	else
		return true;
}

void ModelLoader::verticesFromFaceString(std::vector<Vertex>& outputVertices, const std::vector<glm::vec3>& _positions, const std::vector<glm::vec2>& _texCoords, const std::vector<glm::vec3>& _normals, std::string _curLine)
{
	// substr to remove "f "
	std::vector<std::string> splitFaceStr;
	Vertex vertex;
	Util::split(_curLine.substr(2), ' ', splitFaceStr);

	bool noNormal = false;

	// For every given vertex do this
	for (int i = 0; i < splitFaceStr.size(); i++)
	{
		std::vector<std::string> splitVertexStr;
		Util::split(splitFaceStr[i], '/', splitVertexStr);

		// 1 = pos, 2 = pos/tex, 3 = pos/norm,, 4 = pos/tex/norm
		int vertexType = 0;

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

		splitVertexStr.clear();
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

	splitFaceStr.clear();
}

void ModelLoader::verticesToIndicesTriangulated(std::vector<unsigned int>& indicesOut, const std::vector<Vertex>& _vertices)
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
			float angle = Util::angleBetweenV3(prevVertex.pos - curVertex.pos, nextVertex.pos - curVertex.pos) * (180 / 3.14159265359);
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

	verticesVector.clear();
}