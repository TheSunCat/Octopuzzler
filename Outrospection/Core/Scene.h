#pragma once

#include "Core/World/Character.h"
#include "Core/World/ObjectGeneral.h"
#include "Core/World/PhysicsWorld.h"

class Shader;

class Scene
{
public:
    Scene() = default;

    explicit Scene(const std::string& _name);

    void loadScene();

    void step(float deltaTime);

    void draw(const Camera& cam, Shader& _objShader, Shader& _billboardShader, Shader& _skyShader, Shader& _simpleShader);

    enum class State { Obj, RailObj, Light, Sky, Chara, Col };

    std::string name;

    PhysicsWorld physicsWorld;
    
    std::unordered_map<std::string, std::vector<ObjectGeneral*>> objects;
    std::vector<ObjectGeneral> skies;
    std::vector<Character> characters;

    float voxelWorld[22][22][22];

    float cubeThreshold = 0.24f;
private:
    static DummyObj parseLine(const std::string& line);

    [[nodiscard]] static ObjectGeneral* parseObj(const std::string& line);
    [[nodiscard]] static Character parseChar(const std::string& line);

    void parseCollision(const std::string& name);

    // skybox
    static GLuint loadCubemap(std::string name);
    GLuint cubemapTexture = 0;
    GLuint skyboxVAO = 0;

    // DEBUG
    Mesh colMesh;
};
