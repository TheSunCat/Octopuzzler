#pragma once

#include <glm/glm.hpp>

#include "Types.h"

class Controller;
class Player;

class PlayerController {
public:
	void acceleratePlayer(Player* playerIn, const Controller& controller, float deltaTime);

	void collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData);

	void animatePlayer(Player* playerIn);
	
	void movePlayer(Player* playerIn) const;

private:
	static glm::vec3 processInput(const Controller& controller, const float yaw);
	
	// Return true if a triangle was hit.
	void resolveCollision(Player* playerIn, const std::vector<Triangle>& collisionData);

	glm::vec3 playerVelocity = glm::vec3(0.0f);
	glm::vec3 colResponseDelta = glm::vec3(0.0f); // not carried over loops
	float lastWalkSpeed = 0;
	//glm::vec3 lastGoodPlayerPosition;

	bool grounded = false;
	bool jumping = false;
};