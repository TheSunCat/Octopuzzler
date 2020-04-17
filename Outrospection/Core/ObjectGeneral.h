#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Rendering/Shader.h"
#include "Rendering/Mesh.h"

class ObjectGeneral {
public:
	ObjectGeneral(std::string _name, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scale);

	void draw(const Shader& shader);

	glm::vec3 getPos();

	glm::vec3 getRot();

	glm::vec3 getScale();

private:
	glm::vec3 pos, rot, scale;
	std::vector<Mesh> meshes;

	std::string name;
};