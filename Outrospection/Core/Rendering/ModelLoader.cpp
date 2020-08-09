#include "ModelLoader.h"

#include <fstream>


#include "Outrospection.h"
#include "Util.h"

#include "Core/Rendering/TextureManager.h"

// get element at index position
template <class T>
const T& getElement(const std::vector<T>& elements, std::string_view& indexStr)
{
    int index = Util::stoi(indexStr);

    if (index < 0)
        index = static_cast<int>(elements.size()) + index;
    else
        index--;

    return elements[index];
}

bool ModelLoader::loadFile(const std::string& filePath)
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
    std::vector<GLuint> indices;

    std::vector<std::string> meshMatNames;

    // controls whether the mesh has been named or not. name should be first 'o' or 'g'
    bool meshListening = false;
    std::string meshName;

    bool matListening = false;
    Material tempMaterial;

    std::string curLine;
    while (std::getline(file, curLine))
    {
        if (curLine.empty() || curLine[0] == '#')
            continue;


        switch (curLine[0])
        {
        case 'o': // generate a Mesh Object or Prepare for an object to be created
            {
                if (!meshListening)
                {
                    meshListening = true;

                    // name the mesh
                    meshName = curLine.substr(2);
                }
                else // if(meshListening)
                {
                    // Generate the mesh to put into the array
                    if (!indices.empty() && !vertices.empty()) // if there are verts and indices
                    {
                        // create and insert Mesh
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
                std::vector<std::string_view> vertPosStr;
                Util::split(curLine, ' ', vertPosStr, 2);

                positions.emplace_back(Util::stof(vertPosStr[0]), Util::stof(vertPosStr[1]), Util::stof(vertPosStr[2]));

                vertPosStr.clear();
                break;
            }

        case 't': // Generate a Vertex Texture Coordinate
            {
                std::vector<std::string_view> texCoordStr;
                Util::split(curLine, ' ', texCoordStr, 2);

                texCoords.emplace_back(Util::stof(texCoordStr[0]), Util::stof(texCoordStr[1]));

                texCoordStr.clear();

                break;
            }
            // Generate a Vertex Normal;
        case 'n':
            {
                std::vector<std::string_view> vertNormStr;
                Util::split(curLine, ' ', vertNormStr, 2);

                normals.emplace_back(Util::stof(vertNormStr[0]), Util::stof(vertNormStr[1]),
                                     Util::stof(vertNormStr[2]));
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
                for (Vertex& i : vertsFromFace)
                {
                    vertices.emplace_back(i);
                }

                std::vector<GLuint> indicesVector;

                verticesToIndicesTriangulated(indicesVector, vertsFromFace);

                // Add Indices
                for (GLuint i : indicesVector)
                {
                    GLuint curIndex = static_cast<GLuint>(vertices.size() - vertsFromFace.size()) + i;
                    indices.push_back(curIndex);
                }

                vertsFromFace.clear();
                break;
            }

            // MATERIAL
            // --------
        case 'm':
            {
                switch (curLine[1])
                {
                case 'u': // usemtl
                    {
                        meshMatNames.emplace_back(curLine.substr(3));

                        // Create new Mesh, if Material changes within a group
                        if (!indices.empty() && !vertices.empty())
                        {
                            // Create Mesh
                            loadedMeshes.emplace_back(meshName, vertices, indices);

                            // check for duplicate names
                            int sameNameCount = 0;
                            for (const Mesh& m : loadedMeshes)
                            {
                                if (m.name.starts_with(meshName))
                                    sameNameCount++;
                            }

                            if (sameNameCount > 0)
                                loadedMeshes.back().name = meshName + "_" + std::to_string(sameNameCount);
                            else
                                loadedMeshes.back().name = meshName;

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
                switch (curLine[1])
                {
                case 'd': // Kd, Diffuse Color
                    {
                        std::vector<std::string_view> temp;
                        Util::split(curLine, ' ', temp, 3);

                        if (temp.size() != 3)
                            continue;

                        tempMaterial.colDiffuse.x = Util::stof(temp[0]);
                        tempMaterial.colDiffuse.y = Util::stof(temp[1]);
                        tempMaterial.colDiffuse.z = Util::stof(temp[2]);

                        temp.clear();
                        break;
                    }

                    // Specular Color
                case 's': // Ks, Specular Color
                    {
                        std::vector<std::string_view> temp;
                        Util::split(curLine, ' ', temp, 3);

                        if (temp.size() != 3)
                            continue;

                        tempMaterial.colSpecular.x = Util::stof(temp[0]);
                        tempMaterial.colSpecular.y = Util::stof(temp[1]);
                        tempMaterial.colSpecular.z = Util::stof(temp[2]);

                        temp.clear();
                        break;
                    }

                    // Ambient Color
                case 'a':
                    {
                        std::vector<std::string_view> temp;
                        Util::split(curLine, ' ', temp, 3);

                        if (temp.size() != 3)
                            continue;

                        tempMaterial.colAmbient.x = Util::stof(temp[0]);
                        tempMaterial.colAmbient.y = Util::stof(temp[1]);
                        tempMaterial.colAmbient.z = Util::stof(temp[2]);

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

    file.close();

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

    TextureManager& textureManager = Outrospection::get().textureManager;
    // Set Materials for each Mesh
    for (unsigned int i = 0; i < meshMatNames.size(); i++)
    {
        std::string matName = meshMatNames[i];

        // Find corresponding material name in loaded materials
        // when found copy material variables into mesh material
        for (Material& loadedMaterial : loadedMaterials)
        {
            if (loadedMaterial.name == matName)
            {
                // TODO support more mat stuff (and write it to the Mesh obj)

                Resource r("Textures/", loadedMaterial.mapDiffuse);

                loadedMeshes[i].texture = textureManager.loadTexture(r);
                break;
            }
        }
    }

    if (loadedMeshes.empty())
        return false;
    else
        return true;
}

void ModelLoader::verticesFromFaceString(std::vector<Vertex>& outputVertices, const std::vector<glm::vec3>& _positions,
                                         const std::vector<glm::vec2>& _texCoords,
                                         const std::vector<glm::vec3>& _normals, const std::string& _curLine)
{
    // substr to remove "f "
    std::vector<std::string_view> splitFaceStr;
    Vertex vertex{};
    Util::split(_curLine, ' ', splitFaceStr, 2);

    bool noNormal = false;

    for (std::string_view& i : splitFaceStr)
    {
        std::string strToSplit(i.begin(), i.end());

        std::vector<std::string_view> splitVertexStr;
        Util::split(strToSplit, '/', splitVertexStr);

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
            if (!splitVertexStr[1].empty())
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
                vertex.pos = getElement(_positions, splitVertexStr[0]);
                vertex.texCoord = glm::vec2(0, 0);
                noNormal = true;
                outputVertices.push_back(vertex);
                break;
            }
        case 2: // P/T
            {
                vertex.pos = getElement(_positions, splitVertexStr[0]);
                vertex.texCoord = getElement(_texCoords, splitVertexStr[1]);
                noNormal = true;
                outputVertices.push_back(vertex);
                break;
            }
        case 3: // P//N
            {
                vertex.pos = getElement(_positions, splitVertexStr[0]);
                vertex.texCoord = glm::vec2(0, 0);
                vertex.normal = getElement(_normals, splitVertexStr[2]);
                outputVertices.push_back(vertex);
                break;
            }
        case 4: // P/T/N
            {
                vertex.pos = getElement(_positions, splitVertexStr[0]);
                vertex.texCoord = getElement(_texCoords, splitVertexStr[1]);
                vertex.normal = getElement(_normals, splitVertexStr[2]);
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

        for (Vertex& outputVertex : outputVertices)
        {
            outputVertex.normal = normal;
        }
    }

    splitFaceStr.clear();
}

void ModelLoader::verticesToIndicesTriangulated(std::vector<GLuint>& indicesOut, const std::vector<Vertex>& _vertices)
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
        for (unsigned int vertexIndex = 0; vertexIndex < verticesVector.size(); vertexIndex++)
        {
            Vertex prevVertex = vertexIndex == 0
                                    ? verticesVector[verticesVector.size() - 1]
                                    : verticesVector[vertexIndex - 1];
            Vertex curVertex = verticesVector[vertexIndex];
            Vertex nextVertex = vertexIndex == verticesVector.size() - 1
                                    ? verticesVector[0]
                                    : verticesVector[1 + vertexIndex];

            // 3 verts left means this is the last tri
            if (verticesVector.size() == 3)
            {
                // create tri from current, previous, and next vert
                for (GLuint index = 0; index < verticesVector.size(); index++)
                {
                    if (_vertices[index].pos == curVertex.pos)
                        indicesOut.push_back(index);
                    if (_vertices[index].pos == prevVertex.pos)
                        indicesOut.push_back(index);
                    if (_vertices[index].pos == nextVertex.pos)
                        indicesOut.push_back(index);
                }

                // we're done!
                verticesVector.clear();
                break;
            }

            if (verticesVector.size() == 4)
            {
                // create tri from current, previous, and next vert
                for (GLuint index = 0; index < _vertices.size(); index++)
                {
                    if (_vertices[index].pos == curVertex.pos)
                        indicesOut.push_back(index);
                    if (_vertices[index].pos == prevVertex.pos)
                        indicesOut.push_back(index);
                    if (_vertices[index].pos == nextVertex.pos)
                        indicesOut.push_back(index);
                }

                glm::vec3 tempVec;
                for (Vertex& j : verticesVector)
                {
                    if (j.pos != curVertex.pos
                        && j.pos != prevVertex.pos
                        && j.pos != nextVertex.pos)
                    {
                        tempVec = j.pos;
                        break;
                    }
                }

                // create tri from current, previous, and next vert
                for (GLuint index = 0; index < _vertices.size(); index++)
                {
                    if (_vertices[index].pos == prevVertex.pos)
                        indicesOut.push_back(index);
                    if (_vertices[index].pos == nextVertex.pos)
                        indicesOut.push_back(index);
                    if (_vertices[index].pos == tempVec)
                        indicesOut.push_back(index);
                }

                // we're done!
                verticesVector.clear();
                break;
            }

            // if Vertex is not an interior vertex
            float angle = Util::angleBetweenV3(prevVertex.pos - curVertex.pos, nextVertex.pos - curVertex.pos)
                * (180.0f / 3.14159265359f);
            if (angle <= 0 && angle >= 180)
                continue;

            // if any vertices are within this triangle
            bool inTri = false;
            for (const Vertex& vertex : _vertices)
            {
                if (Util::inTriangle(vertex.pos, Triangle{prevVertex.pos, curVertex.pos, nextVertex.pos})
                    && vertex.pos != prevVertex.pos
                    && vertex.pos != curVertex.pos
                    && vertex.pos != nextVertex.pos)
                {
                    inTri = true;
                    break;
                }
            }

            if (inTri)
                continue;

            // create tri from current, previous, and next vert
            for (GLuint index = 0; index < _vertices.size(); index++)
            {
                if (_vertices[index].pos == curVertex.pos)
                    indicesOut.push_back(index);
                if (_vertices[index].pos == prevVertex.pos)
                    indicesOut.push_back(index);
                if (_vertices[index].pos == nextVertex.pos)
                    indicesOut.push_back(index);
            }

            // remove curVertex from the working list
            for (unsigned int j = 0; j < verticesVector.size(); j++)
            {
                if (verticesVector[j].pos == curVertex.pos)
                {
                    verticesVector.erase(verticesVector.begin() + j);
                    break;
                }
            }

            // reset index to the start (-1 since loop will add 1 to it)
            vertexIndex = -1;
        }

        // no triangles were created
        if (indicesOut.empty())
            break;

        // no more vertices
        if (verticesVector.empty())
            break;
    }

    verticesVector.clear();
}
