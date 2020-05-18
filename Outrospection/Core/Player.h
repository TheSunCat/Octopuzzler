#pragma once

#include <glm/glm.hpp>

#include "Core/Character.h"

class Shader;

class Player {
public:
	Player() = default;

	Player(const glm::vec3& _startPos);

	void move(const glm::vec3& deltaPos);

	//void setAnimation(AnimType _animType);

	void draw(Shader& _shader) const;

	glm::vec3 position{};
	Character character;
	float yaw = 0.0f;
	
	float moveSpeed = 1.5f;
};