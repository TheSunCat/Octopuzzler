#include "Scene.h"

#include <fstream>
#include <iostream>
#include <utility>

#include "External/stb_image.h"

#include "Constants.h"
#include "Util.h"

#include "Core/Rendering/Shader.h"

Scene::Scene(std::string _name)
{
	name = std::move(_name);

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
	GLuint skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
}

void Scene::loadScene()
{
	// parser code partly by MarkCangila

	std::ifstream sceneFile("./res/StageData/" + name + "/" + name + ".lvl");
	State currentState = Scene::State::Obj;

	for (std::string line; getline(sceneFile, line); )
	{
		if (line.empty()) { // skip empty lines
			continue;
		}
		if (line == "Obj") {
			currentState = Scene::State::Obj;
		}
		/*else if (line.compare("RailObj") == 0) {
			currentState = RailObj;
		}*/
		else if (line == "Light") {
			currentState = Scene::State::Light;
		}
		else if (line == "Sky") {
			currentState = Scene::State::Sky;
		}
		else if (line == "Chara") {
			currentState = Scene::State::Chara;
		}
		else if (line == "Col") {
			currentState = Scene::State::Col;
		}
		else {
			switch (currentState) {
			case Scene::State::Obj: {
				// parse object
				objects.emplace_back(parseObj(line));
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
				parseCollision(line);

				if (DEBUG)
				{
					std::vector<Vertex> colVerticesVector;
					for (const Triangle& t : collision) {
						colVerticesVector.push_back(Vertex{ t.v0, t.n });
						colVerticesVector.push_back(Vertex{ t.v1, t.n });
						colVerticesVector.push_back(Vertex{ t.v2, t.n });
					}

					std::vector<GLuint> indices(colVerticesVector.size());
					for (unsigned int i = 0; i < colVerticesVector.size(); i++)
					{
						indices[i] = i;
					}

					colMesh = Mesh("Collision model", colVerticesVector, indices);
				}
			}
			} // end switch(currentState)
		} // end line compare else
	} // end for loop

	sceneFile.close();
}

void Scene::draw(Shader& _objShader, Shader& _billboardShader, Shader& _skyShader, Shader& _simpleShader)
{
	// render sky
	_skyShader.use();
	glDepthMask(GL_FALSE);
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	Util::glError(true);

	_objShader.use();
	

	if (DEBUG)
	{
		_simpleShader.use();
		
		const glm::mat4 modelMat = glm::mat4(1.0f);
		_simpleShader.setMat4("model", modelMat);

		colMesh.draw();
	}
	else {
		for (const ObjectGeneral& object : objects)
		{
			object.draw(_objShader);
		}
	}

	Util::glError(true);

	_billboardShader.use();
	for (const Character& chara : characters)
	{
		chara.draw(_billboardShader);
	}

	Util::glError(true);
}

DummyObj Scene::parseLine(const std::string& line)
{
	const size_t n = std::count(line.begin(), line.end(), '|');

	if (n == 0)
		return  { line };

	DummyObj ret;

	//Split string by property delimiter
	std::vector<std::string_view> splittedLine;
	Util::split(line, '|', splittedLine);
	
	// Object name
	ret.name = splittedLine[0];

	// Object properties
	for (unsigned int i = 0; i < n; i++)
	{
		std::vector<std::string_view> v;
		std::string strToSplit = std::string(splittedLine[i + 1].begin(), splittedLine[i + 1].end());
		Util::split(strToSplit, ' ', v);

		for(std::string_view str : v)
			ret.properties.push_back(Util::stof(str));
	}

	return ret;
}

ObjectGeneral Scene::parseObj(const std::string& line)
{
	DummyObj obj = parseLine(line);

	const std::string_view objName = std::move(obj.name);

	const glm::vec3 pos(obj.properties[0], obj.properties[1], obj.properties[2]);
	const glm::vec3 rot(obj.properties[3], obj.properties[4], obj.properties[5]);
	const glm::vec3 scl(obj.properties[6], obj.properties[7], obj.properties[8]);

	return ObjectGeneral(std::string(objName), pos, rot, scl);
}

Character Scene::parseChar(const std::string& line)
{
	DummyObj obj = parseLine(line);

	const glm::vec3 pos = glm::vec3(obj.properties[0], obj.properties[1], obj.properties[2]);
	
	Character chara(obj.name, pos);

	return chara;
}

void Scene::parseCollision(const std::string& name)
{
	std::ifstream collisionFile("./res/ObjectData/" + name + "/" + name + ".ocl");
	for (std::string line; getline(collisionFile, line);)
	{
		if (line.empty() || line[0] == '#')
			continue;

		std::vector<std::string_view> verticesStr;
		Util::split(line, '|', verticesStr, 0, 1); // cut out extra '|' at the end until I fix exporter lol

		std::vector<glm::vec3> vertices;

		for (const std::string_view& s : verticesStr)
		{
			std::string strToSplit = std::string(s.begin(), s.end());
			std::vector<std::string_view> sStr;
			Util::split(strToSplit, ' ', sStr);

			vertices.emplace_back(Util::stof(sStr[0]), Util::stof(sStr[1]), Util::stof(sStr[2]));
		}

		collision.emplace_back(Triangle{ vertices[0], vertices[1], vertices[2] });
		collision.back().n = Util::genNormal(collision.back());
	}

	collisionFile.close();

	for(auto iter = collision.begin(); iter != collision.end(); ++iter)
	{
		Triangle curTri = *iter;
		if(fabs(curTri.n.y) > 0.1f)
		{
			groundCollision.emplace_back(iter);
		}
		else
		{
			wallCollision.emplace_back(iter);
		}
	}
}

GLuint Scene::loadCubemap(std::string name)
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

	GLuint textureID;
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