#pragma once

#include "Core/World/Character.h"
#include "Core/World/CollisionMesh.h"
#include "Core/World/ObjectGeneral.h"

class Shader;

class Scene
{
public:
    // default ctor
    Scene() = default;

    explicit Scene(const std::string& _name);

    void loadScene();

    void draw(const Camera& cam, Shader& _objShader, Shader& _billboardShader, Shader& _skyShader, Shader& _simpleShader);

    enum class State { Obj, RailObj, Light, Sky, Chara, Col };

    std::string name;

    //PhysicsWorld physics;
    
    std::unordered_map<std::string, std::vector<ObjectGeneral>> objects;
    std::vector<ObjectGeneral> skies;
    std::vector<Character> characters;

    std::vector<CollisionMesh> collision;
    std::vector<std::vector<Triangle>::const_iterator> groundCollision;
    std::vector<std::vector<Triangle>::const_iterator> wallCollision;

    float voxelWorld[22][22][22];

    float cubeThreshold = 0.24f;
private:
    static DummyObj parseLine(const std::string& line);

    [[nodiscard]] static ObjectGeneral parseObj(const std::string& line);
    [[nodiscard]] static Character parseChar(const std::string& line);

    void parseCollision(const std::string& name);

    // skybox
    static GLuint loadCubemap(std::string name);
    GLuint cubemapTexture = 0;
    GLuint skyboxVAO = 0;

    // DEBUG
    Mesh colMesh;
};
