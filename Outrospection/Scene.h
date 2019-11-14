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

	vector<string> parseLine(string line) {
		vector<string> ret;

		//Split string
		vector<string> splittedLine = split(line, " | ");

		//Split position
		vector<string> positions = split(splittedLine[1], " ");
		//Split rotations
		vector<string> rotations = split(splittedLine[2], " ");
		//Split scales
		vector<string> scales = split(splittedLine[3], " ");

		ret.push_back(splittedLine[0]);
		push_all(ret, positions);
		push_all(ret, rotations);
		push_all(ret, scales);

		return ret;
	}

	ObjectGeneral parseObj(vector<string> line) {
		string objName = line[0];

		glm::vec3 pos(stof(line[1]), stof(line[2]), stof(line[3]));

		glm::vec3 rot(stof(line[4]), stof(line[5]), stof(line[6]));

		glm::vec3 scl(stof(line[7]), stof(line[8]), stof(line[9]));

		return ObjectGeneral(objName, pos, rot, scl);
	}
};

#endif