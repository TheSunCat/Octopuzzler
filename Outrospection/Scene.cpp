#include "Scene.h"

Scene::Scene(string _name) {
	name = _name;

	loadScene();
}

void Scene::loadScene() {
	// following parser code by MarkCangila

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
		else if (line.compare("Sky") == 0) {
			currentState = Sky;
		}
		else {
			switch (currentState) {
			case Object: {
				// parse object
				vector<string> objLines = parseLine(line);

				ObjectGeneral obj = parseObj(objLines);

				objects.push_back(obj);
				break;
			}
			case RailObject: {

				break;
			}
			case Light: {
				//Split string
				vector<string> splittedLine = split(line, " | ");
				//Split position
				vector<string> positions = split(splittedLine[1], " ");
				//Split rotations
				vector<string> color = split(splittedLine[2], " ");

				// TODO light support lol
				//lights.push_back
				break;
			}
			case Sky: {
				// parse object
				vector<string> objLines = parseLine(line);

				ObjectGeneral obj = parseObj(objLines);

				skies.push_back(obj);
				break;
			}

			}
		}
	}
}

void Scene::draw(Shader& _objShader, Shader& _billboardShader, Shader& _skyShader) {
	// TODO make skies be behind everything. how? don't ask me!
	_skyShader.use();

	glDepthFunc(GL_GEQUAL);
	for (ObjectGeneral object : skies) {
		object.draw(_skyShader);
	}
	glDepthFunc(GL_LESS);
	
	_objShader.use();
	for (ObjectGeneral object : objects) {
		object.draw(_objShader);
	}
}

vector<string> Scene::parseLine(string line) {
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

ObjectGeneral Scene::parseObj(vector<string> line) {
	string objName = line[0];

	glm::vec3 pos(stof(line[1]), stof(line[2]), stof(line[3]));

	glm::vec3 rot(stof(line[4]), stof(line[5]), stof(line[6]));

	glm::vec3 scl(stof(line[7]), stof(line[8]), stof(line[9]));

	return ObjectGeneral(objName, pos, rot, scl);
}