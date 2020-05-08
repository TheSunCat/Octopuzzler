#pragma once

#include <glm/glm.hpp>

#include "Types.h"

class Controller;
class Player;

class PlayerController {
public:
	void acceleratePlayer(Player* playerIn, const Controller& controller);

	void collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime);

	void animatePlayer(Player* playerIn);
	
	void movePlayer(Player* playerIn, float deltaTime);

private:
	// Return true if a triangle was hit.
	bool resolveCollision(Player* playerIn, const std::vector<Triangle>& collisionData);

	glm::vec3 playerVelocity = glm::vec3(0.0f);
	glm::vec3 velocityShift = glm::vec3(0.0f); // not carried over loops
	//glm::vec3 lastGoodPlayerPosition;

	bool grounded = false;
	bool jumping = false;
};