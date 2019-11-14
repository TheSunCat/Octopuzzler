#include "Scene.h"
#include <string>
#include "ObjectGeneral.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "Util.h"
#include "ObjectRail.h"
#include "ObjectRail.cpp"

using namespace std;

Scene::Scene(string _name) {
	name = _name;

	loadScene();
}

void Scene::loadScene() {
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
				// parse object
				vector<string> objLines = parseLine(line);

				ObjectGeneral obj = parseObj(objLines);

				objects.push_back(ObjectRail(obj));
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

void Scene::draw(Shader shader) {
	for (ObjectRail object : objects) {
		object.draw(shader);
	}
}
