#pragma once

#include <glm/glm.hpp>

#include "Core/Character.h"

class Shader;

class Player {
public:
	Player() = default;

	Player(const glm::vec3& _startPos, const glm::vec3& _startRot);

	void move(const glm::vec3& deltaPos);

	void rotate(float yaw);

	//void setAnimation(AnimType _animType);

	void draw(Shader& objShader);

	glm::vec3 playerPosition;
	glm::vec3 playerRotation;
	Character playerCharacter;

	float playerSpeed = 1;
};