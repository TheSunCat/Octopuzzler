#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include "util.h"

struct Vertex
{
	// Position Vector
	glm::vec3 Position;

	// Normal Vector
	glm::vec3 Normal;

	// Texture Coordinate Vector
	glm::vec2 TextureCoordinate;
};

struct Material
{
	Material()
	{
		name;
		Ns = 0.0f;
		Ni = 0.0f;
		d = 0.0f;
		illum = 0;
	}

	// Material Name
	std::string name;
	// Ambient Color
	glm::vec3 Ka;
	// Diffuse Color
	glm::vec3 Kd;
	// Specular Color
	glm::vec3 Ks;
	// Specular Exponent
	float Ns;
	// Optical Density
	float Ni;
	// Dissolve
	float d;
	// Illumination
	int illum;
	// Ambient Texture Map
	std::string map_Ka;
	// Diffuse Texture Map
	std::string map_Kd;
	// Specular Texture Map
	std::string map_Ks;
	// Specular Hightlight Map
	std::string map_Ns;
	// Alpha Texture Map
	std::string map_d;
	// Bump Map
	std::string map_bump;
};

struct Mesh
{
	Mesh() = default;

	Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices)
	{
		Vertices = _Vertices;
		Indices = _Indices;
	}

	// Mesh Name
	std::string MeshName;
	// Vertex List
	std::vector<Vertex> Vertices;
	// Index List
	std::vector<unsigned int> Indices;

	// Material
	Material MeshMaterial;
};

namespace algorithm
{
	// glm::vec3 Multiplication Opertor Overload
	glm::vec3 operator*(const float& left, const glm::vec3& right)
	{
		return glm::vec3(right.x * left, right.y * left, right.z * left);
	}

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
	inline void split(const std::string& in,
		std::vector<std::string>& out,
		std::string token)
	{
		out.clear();

		std::string temp;

		for (int i = 0; i < int(in.size()); i++)
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
		size_t token_start = in.find_first_not_of(" \t");
		size_t space_start = in.find_first_of(" \t", token_start);
		size_t tail_start = in.find_first_not_of(" \t", space_start);
		size_t tail_end = in.find_last_not_of(" \t");
		if (tail_start != std::string::npos && tail_end != std::string::npos)
		{
			return in.substr(tail_start, tail_end - tail_start + 1);
		}
		else if (tail_start != std::string::npos)
		{
			return in.substr(tail_start);
		}
		return "";
	}

	// Get first token of string
	inline std::string firstToken(const std::string& in)
	{
		if (!in.empty())
		{
			size_t token_start = in.find_first_not_of(" \t");
			size_t token_end = in.find_first_of(" \t", token_start);
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
	inline const T& getElement(const std::vector<T>& elements, std::string& index)
	{
		int idx = std::stoi(index);
		if (idx < 0)
			idx = int(elements.size()) + idx;
		else
			idx--;
		return elements[idx];
	}
}

class ModelLoader {
public:
	bool LoadFile(std::string filePath)
	{
		// If the file is not an .obj file return false
		if (filePath.substr(filePath.size() - 4, 4) != ".obj")
			return false;


		std::ifstream file(filePath);

		if (!file.is_open())
			return false;

		LoadedMeshes.clear();
		LoadedVertices.clear();
		LoadedIndices.clear();

		std::vector<glm::vec3> Positions;
		std::vector<glm::vec2> TCoords;
		std::vector<glm::vec3> Normals;

		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;

		std::vector<std::string> MeshMatNames;

		bool listening = false;
		std::string meshname;

		Mesh tempMesh;

		std::string curline;
		while (std::getline(file, curline)) {

			// Generate a Mesh Object or Prepare for an object to be created
			if (algorithm::firstToken(curline) == "o" || algorithm::firstToken(curline) == "g" || curline[0] == 'g')
			{
				if (!listening)
				{
					listening = true;

					if (algorithm::firstToken(curline) == "o" || algorithm::firstToken(curline) == "g")
					{
						meshname = algorithm::tail(curline);
					}
					else
					{
						meshname = "unnamed";
					}
				}
				else
				{
					// Generate the mesh to put into the array

					if (!Indices.empty() && !Vertices.empty())
					{
						// Create Mesh
						tempMesh = Mesh(Vertices, Indices);
						tempMesh.MeshName = meshname;

						// Insert Mesh
						LoadedMeshes.push_back(tempMesh);

						// Cleanup
						Vertices.clear();
						Indices.clear();
						meshname.clear();

						meshname = algorithm::tail(curline);
					}
					else
					{
						if (algorithm::firstToken(curline) == "o" || algorithm::firstToken(curline) == "g")
						{
							meshname = algorithm::tail(curline);
						}
						else
						{
							meshname = "unnamed";
						}
					}
				}
			}

			// Generate a Vertex Position
			if (algorithm::firstToken(curline) == "v")
			{
				std::vector<std::string> spos;
				glm::vec3 vpos;
				algorithm::split(algorithm::tail(curline), spos, " ");

				vpos.x = std::stof(spos[0]);
				vpos.y = std::stof(spos[1]);
				vpos.z = std::stof(spos[2]);

				Positions.push_back(vpos);
			}

			// Generate a Vertex Texture Coordinate
			if (algorithm::firstToken(curline) == "vt")
			{
				std::vector<std::string> stex;
				glm::vec2 vtex;
				algorithm::split(algorithm::tail(curline), stex, " ");

				vtex.x = std::stof(stex[0]);
				vtex.y = std::stof(stex[1]);

				TCoords.push_back(vtex);
			}

			// Generate a Vertex Normal;
			if (algorithm::firstToken(curline) == "vn")
			{
				std::vector<std::string> snor;
				glm::vec3 vnor;
				algorithm::split(algorithm::tail(curline), snor, " ");

				vnor.x = std::stof(snor[0]);
				vnor.y = std::stof(snor[1]);
				vnor.z = std::stof(snor[2]);

				Normals.push_back(vnor);
			}

			// Generate a Face (vertices & indices)
			if (algorithm::firstToken(curline) == "f")
			{
				// Generate the vertices
				std::vector<Vertex> vVerts;
				GenVerticesFromRawOBJ(vVerts, Positions, TCoords, Normals, curline);

				// Add Vertices
				for (int i = 0; i < int(vVerts.size()); i++)
				{
					Vertices.push_back(vVerts[i]);

					LoadedVertices.push_back(vVerts[i]);
				}

				std::vector<unsigned int> iIndices;

				VertexTriangluation(iIndices, vVerts);

				// Add Indices
				for (int i = 0; i < int(iIndices.size()); i++)
				{
					unsigned int indnum = (unsigned int)((Vertices.size()) - vVerts.size()) + iIndices[i];
					Indices.push_back(indnum);

					indnum = (unsigned int)((LoadedVertices.size()) - vVerts.size()) + iIndices[i];
					LoadedIndices.push_back(indnum);

				}
			}

			// Get Mesh Material Name
			if (algorithm::firstToken(curline) == "usemtl")
			{
				MeshMatNames.push_back(algorithm::tail(curline));

				// Create new Mesh, if Material changes within a group
				if (!Indices.empty() && !Vertices.empty())
				{
					// Create Mesh
					tempMesh = Mesh(Vertices, Indices);
					tempMesh.MeshName = meshname;
					int i = 2;
					while (1) {
						tempMesh.MeshName = meshname + "_" + std::to_string(i);

						for (auto& m : LoadedMeshes)
							if (m.MeshName == tempMesh.MeshName)
								continue;
						break;
					}

					// Insert Mesh
					LoadedMeshes.push_back(tempMesh);

					// Cleanup
					Vertices.clear();
					Indices.clear();
				}
			}

			// Load Materials
			if (algorithm::firstToken(curline) == "mtllib")
			{
				// Generate LoadedMaterial

				// Generate a path to the material file
				std::vector<std::string> temp;
				algorithm::split(filePath, temp, "/");

				std::string pathtomat = "";

				if (temp.size() != 1)
				{
					for (int i = 0; i < temp.size() - 1; i++)
					{
						pathtomat += temp[i] + "/";
					}
				}


				pathtomat += algorithm::tail(curline);

				// Load Materials
				LoadMaterials(pathtomat);
			}
		}

		// Deal with last mesh
		if (!Indices.empty() && !Vertices.empty())
		{
			// Create Mesh
			tempMesh = Mesh(Vertices, Indices);
			tempMesh.MeshName = meshname;

			// Insert Mesh
			LoadedMeshes.push_back(tempMesh);
		}

		file.close();

		// Set Materials for each Mesh
		for (int i = 0; i < MeshMatNames.size(); i++)
		{
			std::string matname = MeshMatNames[i];

			// Find corresponding material name in loaded materials
			// when found copy material variables into mesh material
			for (int j = 0; j < LoadedMaterials.size(); j++)
			{
				if (LoadedMaterials[j].name == matname)
				{
					LoadedMeshes[i].MeshMaterial = LoadedMaterials[j];
					break;
				}
			}
		}

		if (LoadedMeshes.empty() && LoadedVertices.empty() && LoadedIndices.empty())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	// Loaded Mesh Objects
	std::vector<Mesh> LoadedMeshes;
	// Loaded Vertex Objects
	std::vector<Vertex> LoadedVertices;
	// Loaded Index Positions
	std::vector<unsigned int> LoadedIndices;
	// Loaded Material Objects
	std::vector<Material> LoadedMaterials;

private:
	// Generate vertices from a list of positions, 
	//	tcoords, normals and a face line
	void GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
		const std::vector<glm::vec3>& iPositions,
		const std::vector<glm::vec2>& iTCoords,
		const std::vector<glm::vec3>& iNormals,
		std::string icurline)
	{
		std::vector<std::string> sface, svert;
		Vertex vVert;
		algorithm::split(algorithm::tail(icurline), sface, " ");

		bool noNormal = false;

		// For every given vertex do this
		for (int i = 0; i < int(sface.size()); i++)
		{
			// See What type the vertex is.
			int vtype;

			algorithm::split(sface[i], svert, "/");

			// Check for just position - v1
			if (svert.size() == 1)
			{
				// Only position
				vtype = 1;
			}

			// Check for position & texture - v1/vt1
			if (svert.size() == 2)
			{
				// Position & Texture
				vtype = 2;
			}

			// Check for Position, Texture and Normal - v1/vt1/vn1
			// or if Position and Normal - v1//vn1
			if (svert.size() == 3)
			{
				if (svert[1] != "")
				{
					// Position, Texture, and Normal
					vtype = 4;
				}
				else
				{
					// Position & Normal
					vtype = 3;
				}
			}

			// Calculate and store the vertex
			switch (vtype)
			{
			case 1: // P
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = glm::vec2(0, 0);
				noNormal = true;
				oVerts.push_back(vVert);
				break;
			}
			case 2: // P/T
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = algorithm::getElement(iTCoords, svert[1]);
				noNormal = true;
				oVerts.push_back(vVert);
				break;
			}
			case 3: // P//N
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = glm::vec2(0, 0);
				vVert.Normal = algorithm::getElement(iNormals, svert[2]);
				oVerts.push_back(vVert);
				break;
			}
			case 4: // P/T/N
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = algorithm::getElement(iTCoords, svert[1]);
				vVert.Normal = algorithm::getElement(iNormals, svert[2]);
				oVerts.push_back(vVert);
				break;
			}
			default:
			{
				break;
			}
			}
		}

		// take care of missing normals
		// these may not be truly accurate but it is the 
		// best they get for not compiling a mesh with normals	
		if (noNormal)
		{
			glm::vec3 A = oVerts[0].Position - oVerts[1].Position;
			glm::vec3 B = oVerts[2].Position - oVerts[1].Position;

			glm::vec3 normal = glm::cross(A, B);

			for (int i = 0; i < int(oVerts.size()); i++)
			{
				oVerts[i].Normal = normal;
			}
		}
	}

	// Triangulate a list of vertices into a face by printing
	// indicies corresponding with triangles within it
	void VertexTriangluation(std::vector<unsigned int>& oIndices,
		const std::vector<Vertex>& iVerts)
	{
		// If there are 2 or less verts,
		// no triangle can be created,
		// so exit
		if (iVerts.size() < 3)
		{
			return;
		}
		// If it is a triangle no need to calculate it
		if (iVerts.size() == 3)
		{
			oIndices.push_back(0);
			oIndices.push_back(1);
			oIndices.push_back(2);
			return;
		}

		// Create a list of vertices
		std::vector<Vertex> tVerts = iVerts;

		while (true)
		{
			// For every vertex
			for (int i = 0; i < int(tVerts.size()); i++)
			{
				// pPrev = the previous vertex in the list
				Vertex pPrev;
				if (i == 0)
				{
					pPrev = tVerts[tVerts.size() - 1];
				}
				else
				{
					pPrev = tVerts[i - 1];
				}

				// pCur = the current vertex;
				Vertex pCur = tVerts[i];

				// pNext = the next vertex in the list
				Vertex pNext;
				if (i == tVerts.size() - 1)
				{
					pNext = tVerts[0];
				}
				else
				{
					pNext = tVerts[i + 1];
				}

				// Check to see if there are only 3 verts left
				// if so this is the last triangle
				if (tVerts.size() == 3)
				{
					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(tVerts.size()); j++)
					{
						if (iVerts[j].Position == pCur.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
					}

					tVerts.clear();
					break;
				}
				if (tVerts.size() == 4)
				{
					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(iVerts.size()); j++)
					{
						if (iVerts[j].Position == pCur.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
					}

					glm::vec3 tempVec;
					for (int j = 0; j < int(tVerts.size()); j++)
					{
						if (tVerts[j].Position != pCur.Position
							&& tVerts[j].Position != pPrev.Position
							&& tVerts[j].Position != pNext.Position)
						{
							tempVec = tVerts[j].Position;
							break;
						}
					}

					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(iVerts.size()); j++)
					{
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == tempVec)
							oIndices.push_back(j);
					}

					tVerts.clear();
					break;
				}

				// If Vertex is not an interior vertex
				float angle = angleBetweenV3(pPrev.Position - pCur.Position, pNext.Position - pCur.Position) * (180 / 3.14159265359);
				if (angle <= 0 && angle >= 180)
					continue;

				// If any vertices are within this triangle
				bool inTri = false;
				for (int j = 0; j < int(iVerts.size()); j++)
				{
					if (algorithm::inTriangle(iVerts[j].Position, pPrev.Position, pCur.Position, pNext.Position)
						&& iVerts[j].Position != pPrev.Position
						&& iVerts[j].Position != pCur.Position
						&& iVerts[j].Position != pNext.Position)
					{
						inTri = true;
						break;
					}
				}
				if (inTri)
					continue;

				// Create a triangle from pCur, pPrev, pNext
				for (int j = 0; j < int(iVerts.size()); j++)
				{
					if (iVerts[j].Position == pCur.Position)
						oIndices.push_back(j);
					if (iVerts[j].Position == pPrev.Position)
						oIndices.push_back(j);
					if (iVerts[j].Position == pNext.Position)
						oIndices.push_back(j);
				}

				// Delete pCur from the list
				for (int j = 0; j < int(tVerts.size()); j++)
				{
					if (tVerts[j].Position == pCur.Position)
					{
						tVerts.erase(tVerts.begin() + j);
						break;
					}
				}

				// reset i to the start
				// -1 since loop will add 1 to it
				i = -1;
			}

			// if no triangles were created
			if (oIndices.size() == 0)
				break;

			// if no more vertices
			if (tVerts.size() == 0)
				break;
		}
	}

	// Load Materials from .mtl file
	bool LoadMaterials(std::string path)
	{
		// If the file is not a material file return false
		if (path.substr(path.size() - 4, path.size()) != ".mtl")
			return false;

		std::ifstream file(path);

		// If the file is not found return false
		if (!file.is_open())
			return false;

		Material tempMaterial;

		bool listening = false;

		// Go through each line looking for material variables
		std::string curline;
		while (std::getline(file, curline))
		{
			// new material and material name
			if (algorithm::firstToken(curline) == "newmtl")
			{
				if (!listening)
				{
					listening = true;

					if (curline.size() > 7)
					{
						tempMaterial.name = algorithm::tail(curline);
					}
					else
					{
						tempMaterial.name = "none";
					}
				}
				else
				{
					// Generate the material

					// Push Back loaded Material
					LoadedMaterials.push_back(tempMaterial);

					// Clear Loaded Material
					tempMaterial = Material();

					if (curline.size() > 7)
					{
						tempMaterial.name = algorithm::tail(curline);
					}
					else
					{
						tempMaterial.name = "none";
					}
				}
			}
			// Ambient Color
			if (algorithm::firstToken(curline) == "Ka")
			{
				std::vector<std::string> temp;
				algorithm::split(algorithm::tail(curline), temp, " ");

				if (temp.size() != 3)
					continue;

				tempMaterial.Ka.x = std::stof(temp[0]);
				tempMaterial.Ka.y = std::stof(temp[1]);
				tempMaterial.Ka.z = std::stof(temp[2]);
			}
			// Diffuse Color
			if (algorithm::firstToken(curline) == "Kd")
			{
				std::vector<std::string> temp;
				algorithm::split(algorithm::tail(curline), temp, " ");

				if (temp.size() != 3)
					continue;

				tempMaterial.Kd.x = std::stof(temp[0]);
				tempMaterial.Kd.y = std::stof(temp[1]);
				tempMaterial.Kd.z = std::stof(temp[2]);
			}
			// Specular Color
			if (algorithm::firstToken(curline) == "Ks")
			{
				std::vector<std::string> temp;
				algorithm::split(algorithm::tail(curline), temp, " ");

				if (temp.size() != 3)
					continue;

				tempMaterial.Ks.x = std::stof(temp[0]);
				tempMaterial.Ks.y = std::stof(temp[1]);
				tempMaterial.Ks.z = std::stof(temp[2]);
			}
			// Specular Exponent
			if (algorithm::firstToken(curline) == "Ns")
			{
				tempMaterial.Ns = std::stof(algorithm::tail(curline));
			}
			// Optical Density
			if (algorithm::firstToken(curline) == "Ni")
			{
				tempMaterial.Ni = std::stof(algorithm::tail(curline));
			}
			// Dissolve
			if (algorithm::firstToken(curline) == "d")
			{
				tempMaterial.d = std::stof(algorithm::tail(curline));
			}
			// Illumination
			if (algorithm::firstToken(curline) == "illum")
			{
				tempMaterial.illum = std::stoi(algorithm::tail(curline));
			}
			// Ambient Texture Map
			if (algorithm::firstToken(curline) == "map_Ka")
			{
				tempMaterial.map_Ka = algorithm::tail(curline);
			}
			// Diffuse Texture Map
			if (algorithm::firstToken(curline) == "map_Kd")
			{
				tempMaterial.map_Kd = algorithm::tail(curline);
			}
			// Specular Texture Map
			if (algorithm::firstToken(curline) == "map_Ks")
			{
				tempMaterial.map_Ks = algorithm::tail(curline);
			}
			// Specular Hightlight Map
			if (algorithm::firstToken(curline) == "map_Ns")
			{
				tempMaterial.map_Ns = algorithm::tail(curline);
			}
			// Alpha Texture Map
			if (algorithm::firstToken(curline) == "map_d")
			{
				tempMaterial.map_d = algorithm::tail(curline);
			}
			// Bump Map
			if (algorithm::firstToken(curline) == "map_Bump" || algorithm::firstToken(curline) == "map_bump" || algorithm::firstToken(curline) == "bump")
			{
				tempMaterial.map_bump = algorithm::tail(curline);
			}
		}

		// Deal with last material

		// Push Back loaded Material
		LoadedMaterials.push_back(tempMaterial);

		// Test to see if anything was loaded
		// If not return false
		if (LoadedMaterials.empty())
			return false;
		// If so return true
		else
			return true;
	}
};