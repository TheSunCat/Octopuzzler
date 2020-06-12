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
	std::vector<std::vector<Triangle>::const_iterator> groundCollision;
	std::vector<std::vector<Triangle>::const_iterator> wallCollision;
private:
	static DummyObj parseLine(const std::string& line);

	[[nodiscard]] ObjectGeneral parseObj(const std::string& line) const;
	[[nodiscard]] Character parseChar(const std::string& line) const;

	void parseCollision(const std::string& name);

	// skybox
	static GLuint loadCubemap(std::string name);
	GLuint cubemapTexture = 0;
	GLuint skyboxVAO = 0;

	// DEBUG
	Mesh colMesh;
};