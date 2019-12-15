#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Util.h"
#include "KeyBindings.h"

class PlayerController {
public:
	void updatePlayer(Player* playerIn, float deltaTime);

	void updatePhysics(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime);

	void moveForward(Player* playerIn, float deltaTime);

	void moveBackward(Player* playerIn, float deltaTime);

	void moveLeft(Player* playerIn, float deltaTime);

	void moveRight(Player* playerIn, float deltaTime);

private:
	glm::vec3 playerVelocity;

	bool isGrounded = false;
	bool jumping = false;
};