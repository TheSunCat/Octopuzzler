#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Shader.h"

using namespace std;

class ObjectGeneral {
public:
	ObjectGeneral(string _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale);

	void draw(const Shader& shader);

	glm::vec3 getPos();

	glm::vec3 getRot();

	glm::vec3 getScale();

private:
	glm::vec3 pos, rot, scale;
	string name;
};