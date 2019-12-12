#pragma once

#include <string>
#include "ObjectGeneral.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "ObjectRail.h"
#include "Character.h"

class Scene {
public:
	// default ctor
	Scene() = default;

	Scene(string _name);

	void loadScene();

	void draw(Shader& _objShader, Shader& _billboardShader, Shader& _skyShader);

	enum State {Obj, RailObj, Light, Sky, Chara};

	string name;

	vector<ObjectGeneral> objects;
	vector<ObjectRail> railObjs;
	vector<ObjectGeneral> skies;
	vector<Character> characters;
private:
	vector<string> parseLine(string line);

	ObjectGeneral parseObj(string line);
	Character parseChar(string line);

	// skybox
	unsigned int loadCubemap(std::string name);
	unsigned int cubemapTexture;
	unsigned int skyboxVAO;
};