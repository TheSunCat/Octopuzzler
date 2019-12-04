#pragma once
#include <string>
#include <glm/glm.hpp>
#include "Shader.h"

class Character {
public:
	Character() = default;

	Character(const std::string& _charId);


	void draw(Shader& _shader);
private:
	glm::vec3 charPosition;
	glm::vec3 charRotation;

	std::string charId;

	unsigned int charTexture;
	unsigned int quadVAO, quadVBO;
};