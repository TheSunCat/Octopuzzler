#ifndef SCENE_H
#define SCENE_H

#include <string>
#include "ObjectGeneral.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "util.h"
#include "ObjectRail.h"

using namespace std;
class Scene {
public:
	Scene(string _name) {
		name = _name;

		loadScene();
	}

	void loadScene() {
		// following parser code by Keon Sanavandi

		ifstream sceneFile("./res/StageData/" + name + "/" + name + ".lvl");
		State currentState = Object;
		for (std::string line; getline(sceneFile, line); )
		{
			cout << line + "\n";
			if (line.compare("Object") == 0) {
				currentState = Object;
			}
			else if (line.compare("RailObject") == 0) {
				currentState = RailObject;
			}
			else if (line.compare("Light") == 0) {
				currentState = Light;
			}
			else {
				switch (currentState) {
				case Object: {
					//Split string
					vector<string> splittedLine = split(line, " | ");
					cout << splittedLine.size() << endl;
					cout << splittedLine[0] << endl;
					//Split position
					vector<string> positions = split(splittedLine[1], " ");
					//Split rotations
					vector<string> rotations = split(splittedLine[2], " ");
					//Split scales
					vector<string> scales = split(splittedLine[3], " ");
					objects.push_back(ObjectRail(ObjectGeneral(splittedLine[0], glm::vec3(stof(positions[0]), stof(positions[1]), stof(positions[2])), glm::vec3(stof(rotations[0]), stof(rotations[1]), stof(rotations[2])), glm::vec3(stof(scales[0]), stof(scales[1]), stof(scales[2])))));
				}
				case RailObject: {
					
				}
				case Light: {
					//Split string
					vector<string> splittedLine = split(line, " | ");
					//Split position
					vector<string> positions = split(splittedLine[1], " ");
					//Split rotations
					vector<string> color = split(splittedLine[2], " ");
				}

				}
			}
		}
	}

	void draw(Shader shader) {
		for (ObjectGeneral object : objects) {
			object.draw(shader);
		}
	}

private:
	enum State {Object, RailObject, Light};

	string name;

	vector<ObjectGeneral> objects;

	vector<string> parseLine(string line) {
		vector<string> ret();

		return ret;
	}
};

#endif