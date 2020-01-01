#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Util.h"
#include "KeyBindings.h"

class PlayerController {
public:
	void acceleratePlayer(Player* playerIn);

	void collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime);
	
	void movePlayer(Player* playerIn);

private:
	glm::vec3 playerVelocity;

	bool isGrounded = false;
	bool jumping = false;
};