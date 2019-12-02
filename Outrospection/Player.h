#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Character.h"

class Player {
public:
	Player() = default;

	Player(const glm::vec3& _startPos, const glm::vec3& _startRot);

	void move(const glm::vec3& deltaPos);

	void rotate(float yaw);

	void draw(Shader& objShader);

	glm::vec3 playerPosition;
	glm::vec3 playerRotation;
	Character playerCharacter;
};