#pragma once

#include <glm/glm.hpp>

#include "Core/Character.h"

class Shader;

class Player {
public:
	Player() = default;

	explicit Player(const glm::vec3& _startPos);

	void move(const glm::vec3& deltaPos);

	//void setAnimation(AnimType _animType);

	void draw(Shader& _shader) const;

	glm::vec3 position{};
	Character character;

	float eyeHeight = 0.7f;
	float moveSpeed = 1.5f;
};