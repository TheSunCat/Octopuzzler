#include "Scene.h"

#include "Util.h"
#include "External/stb_image.h"
#include "Constants.h"

Scene::Scene(std::string _name) {
	name = _name;

	loadScene();

	// create skybox
	
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	unsigned int skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Scene::loadScene() {
	// parser code partly by MarkCangila

	std::ifstream sceneFile("./res/StageData/" + name + "/" + name + ".lvl");
	State currentState = Scene::State::Obj;

	for (std::string line; getline(sceneFile, line); )
	{
		if (line.compare("") == 0) { // skip empty lines
			continue;
		}
		if (line.compare("Obj") == 0) {
			currentState = Scene::State::Obj;
		}
		/*else if (line.compare("RailObj") == 0) {
			currentState = RailObj;
		}*/
		else if (line.compare("Light") == 0) {
			currentState = Scene::State::Light;
		}
		else if (line.compare("Sky") == 0) {
			currentState = Scene::State::Sky;
		}
		else if (line.compare("Chara") == 0) {
			currentState = Scene::State::Chara;
		}
		else if (line.compare("Col") == 0) {
			currentState = Scene::State::Col;
		}
		else {
			switch (currentState) {
			case Scene::State::Obj: {
				// parse object
				objects.emplace_back(parseObj(line));
				break;
			}
			case Scene::State::RailObj: {
				break;
			}
			case Scene::State::Light: {
				//Split string
				std::vector<std::string> splittedLine;
				Util::split(line, '|', splittedLine);

				//Split position
				std::vector<std::string> positions;
				Util::split(splittedLine[1].substr(1, splittedLine[1].length() - 2), ' ', positions);

				//Split color
				std::vector<std::string> color;
				Util::split(splittedLine[2].substr(1, splittedLine[2].length() - 2), ' ', color);

				// TODO light support lol
				//lights.push_back
				break;
			}
			case Scene::State::Sky: {
				cubemapTexture = loadCubemap(line);
				break;
			}
			case Scene::State::Chara: {
				characters.emplace_back(parseChar(line));
			}
			case Scene::State::Col: {
				// TODO emplace_back?

				parseCollision(line);

				if (DEBUG) {
					std::vector<Vertex> colVerticesVector;
					for (const Triangle& t : collision) {
						colVerticesVector.push_back(Vertex{ t.v0, t.n });
						colVerticesVector.push_back(Vertex{ t.v1, t.n });
						colVerticesVector.push_back(Vertex{ t.v2, t.n });
					}

					std::vector<unsigned int> indices(colVerticesVector.size());
					for (int i = 0; i < colVerticesVector.size(); i++)
					{
						indices[i] = i;
					}

					colMesh = Mesh("Collision model", colVerticesVector, indices);
				}
			}
			}
		}
	}

	sceneFile.close();
}

void Scene::draw(Shader& _objShader, Shader& _billboardShader, Shader& _skyShader, Shader& _simpleShader) {
	// render sky
	_skyShader.use();
	glDepthMask(GL_FALSE);
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	Util::glError(true);

	_objShader.use();
	

	if (DEBUG) {
		glm::mat4 modelMat = glm::mat4(1.0f);
		_objShader.setMat4("model", modelMat);

		colMesh.draw(_objShader);
	}
	else {
		for (const ObjectGeneral& object : objects) {
			object.draw(_objShader);
		}
	}

	Util::glError(true);

	_billboardShader.use();
	for (const Character& chara : characters) {
		chara.draw(_billboardShader);
	}

	Util::glError(true);
}

std::vector<std::string> Scene::parseLine(std::string line) { // TODO do not hardcode number of elements CHECK
	size_t n = std::count(line.begin(), line.end(), '|');

	if (n == 0)
		return  { line };

	std::vector<std::string> ret;

	//Split string by property delimiter
	std::vector<std::string> splittedLine;
	Util::split(line, '|', splittedLine);
	
	// Object name
	ret.push_back(splittedLine[0]);

	// Object properties
	for (int i = 0; i < n; i++) {
		std::vector<std::string> v;
		Util::split(splittedLine[i + 1], ' ', v);

		Util::push_all(ret, v);
	}

	return ret;
}

ObjectGeneral Scene::parseObj(std::string line) {
	std::vector<std::string> lines = parseLine(line);

	std::string objName = lines[0];

	glm::vec3 pos(stof(lines[1]), stof(lines[2]), stof(lines[3]));

	glm::vec3 rot(stof(lines[4]), stof(lines[5]), stof(lines[6]));

	glm::vec3 scl(stof(lines[7]), stof(lines[8]), stof(lines[9]));

	return ObjectGeneral(objName, pos, rot, scl);
}

Character Scene::parseChar(std::string line)
{
	std::vector<std::string> lines = parseLine(line);
	
	glm::vec3 pos = glm::vec3(stof(lines[1]), stof(lines[2]), stof(lines[3]));
	
	Character chara(lines[0], pos);// , { Animation{ AnimType::idle, 0, 1 } });

	return chara;
}

void Scene::parseCollision(std::string name)
{
	std::ifstream collisionFile("./res/ObjectData/" + name + "/" + name + ".ocl");
	for (std::string line; getline(collisionFile, line);)
	{
		if (line == "" || line[0] == '#')
			continue;

		std::vector<std::string> verticesStr;
		Util::split(line.substr(0, line.length() - 1), '|', verticesStr); // cut out extra '|' at the end until I fix exporter lol

		std::vector<glm::vec3> vertices;

		for (const std::string& s : verticesStr) {
			std::vector<std::string> sStr;
			Util::split(s, ' ', sStr);

			vertices.emplace_back(stof(sStr[0]), stof(sStr[1]), stof(sStr[2]));
		}

		collision.emplace_back(Triangle{ vertices[0], vertices[1], vertices[2] });
		collision.back().n = Util::getNormal(collision.back());
	}

	collisionFile.close();
}

unsigned int Scene::loadCubemap(std::string name)
{
	name = "./res/ObjectData/" + name + "/";

	std::vector<std::string> faces = {
		name + "right.png",
		name + "left.png",
		name + "top.png",
		name + "bottom.png",
		name + "front.png",
		name + "back.png"
	};

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}