#ifndef SCENE_H
#define SCENE_H

#include <string>
#include "ObjectGeneral.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "util.h"

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
						objects.push_back(ObjectGeneral(splittedLine[0], glm::vec3(stoi(positions[0]), stoi(positions[1]), stoi(positions[2])), glm::vec3(stoi(rotations[0]), stoi(rotations[1]), stoi(rotations[2])), glm::vec3(stoi(scales[0]), stoi(scales[1]), stoi(scales[2]))));
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
	enum State {Object, Light};

	string name;

	vector<ObjectGeneral> objects;
};

#endif