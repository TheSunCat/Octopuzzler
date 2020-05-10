#pragma once

#include "Core/ObjectGeneral.h"
#include "Core/Character.h"

class Shader;

class Scene {
public:
	// default ctor
	Scene() = default;

	Scene(std::string _name);

	void loadScene();

	void draw(Shader& _objShader, Shader& _billboardShader, Shader& _skyShader, Shader& _simpleShader);

	enum class State {Obj, RailObj, Light, Sky, Chara, Col};

	std::string name;

	std::vector<ObjectGeneral> objects;
	std::vector<ObjectGeneral> skies;
	std::vector<Character> characters;
	std::vector<Triangle> collision;
private:
	static std::vector<std::string> parseLine(std::string line);

	ObjectGeneral parseObj(const std::string& line) const;
	Character parseChar(const std::string& line) const;

	void parseCollision(const std::string& name);

	// skybox
	static unsigned int loadCubemap(std::string name);
	unsigned int cubemapTexture;
	unsigned int skyboxVAO;

	// DEBUG
	Mesh colMesh;
};