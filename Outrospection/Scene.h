#pragma once

#include <string>
#include "ObjectGeneral.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "Util.h"
#include "ObjectRail.h"

using namespace std;
class Scene {
public:
	// default ctor
	Scene() = default;

	Scene(string _name);

	void loadScene();

	void draw(Shader& _objShader, Shader& _billboardShader, Shader& _skyShader);

	enum State {Object, RailObject, Light, Sky};

	string name;

	vector<ObjectGeneral> objects;
	vector<ObjectGeneral> railObjs;
	vector<ObjectGeneral> skies;

	vector<string> parseLine(string line);

	ObjectGeneral parseObj(vector<string> line);
};