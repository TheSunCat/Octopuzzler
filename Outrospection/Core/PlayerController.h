#pragma once

#include <glm/glm.hpp>

#include "Types.h"

class Controller;
class Player;

class PlayerController {
public:
	void acceleratePlayer(const Controller& controller, float deltaTime, float yaw);

	void collidePlayer(Player& player, const std::vector<Triangle>& collisionData);

	void animatePlayer(Player& player);
	
	void movePlayer(Player& player) const;

	bool isMoving() const;
private:
	static glm::vec3 processInput(const Controller& controller, float yaw);
	
	// Return true if a triangle was hit.
	void resolveCollision(Player& player, const std::vector<Triangle>& collisionData);

	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 colResponseDelta = glm::vec3(0.0f); // delta not carried over ticks

	bool grounded = false;
	bool jumping = false;
};