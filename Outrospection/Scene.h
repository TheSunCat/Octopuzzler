#ifndef SCENE_H
#define SCENE_H

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
	Scene(string _name);

	void loadScene();

	void draw(Shader shader);

	enum State {Object, RailObject, Light};

	string name;

	vector<ObjectRail> objects;

	vector<string> parseLine(string line);

	ObjectGeneral parseObj(vector<string> line);
};

#endif