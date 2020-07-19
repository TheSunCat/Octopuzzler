#pragma once

#include <string>
#include <vector>

#include <deque>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Types.h"

struct Vertex;

struct Material {
    Material(): colAmbient(0.0f), colDiffuse(0.0f), colSpecular(0.0f)
    {
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

class ModelLoader {
public:
    // load new file, replacing all fields
    bool loadFile(const std::string& filePath);

    std::deque<Mesh> loadedMeshes;
    std::vector<Material> loadedMaterials;

private:
    // generate vertices from positions, tcoords, normals and a face string
    static void verticesFromFaceString(std::vector<Vertex>& outputVertices, const std::vector<glm::vec3>& _positions,
                                       const std::vector<glm::vec2>& _texCoords, const std::vector<glm::vec3>& _normals,
                                       const std::string& _curLine);

    // triangulate a list of vertices into a face
    static void verticesToIndicesTriangulated(std::vector<GLuint>& indicesOut, const std::vector<Vertex>& _vertices);
};