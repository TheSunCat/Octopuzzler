#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Util.h"
#include "KeyBindings.h"

class PlayerController {
public:
	void acceleratePlayer(Player* playerIn);

	void collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime);

	void animatePlayer(Player* playerIn);
	
	void movePlayer(Player* playerIn, float deltaTime);

private:
	RayHit cast(Ray r, const std::vector<Triangle>& collisionData);
	
	// Return true if a triangle was hit.
	bool resolveCollision(Player* playerIn, const std::vector<Triangle>& collisionData);

	glm::vec3 playerVelocity;

	bool isGrounded = false;
	
	AnimType pastAnim;
};