#include "PlayerController.h"
#include "Constants.h"

void PlayerController::updatePlayer(Player* playerIn, float deltaTime)
{
	if (keyJump && isGrounded) {
		playerIn->move(glm::vec3(0, -GRAVITY / 4, 0));
		//playerIn->setAnimation(1);
	}

	if (keyForward) {
		moveForward(playerIn, deltaTime);
	}

	if (keyBackward) {
		moveBackward(playerIn, deltaTime);
	}

	if (keyLeft) {
		moveLeft(playerIn, deltaTime);
	}

	if (keyRight) {
		moveRight(playerIn, deltaTime);
	}

	if (keyAttack) {
		playerIn->move(glm::vec3(0,  GRAVITY / 4, 0));
	}
}

void PlayerController::updatePhysics(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime)
{
	isGrounded = true;
	// gravity
	//playerVelocity += glm::vec3(0, GRAVITY, 0);

	// collision calculations
	// ----------------------

	// get the player's down-facing ray
	Ray downRay = Ray{ playerIn->playerPosition, glm::vec3(0.0, 1.0, 0.0) };

	float cast = rayCast(downRay, Triangle { glm::vec3(-1.0, 2.0, -1.0), glm::vec3(-1.0, 2.0, 1.0), glm::vec3(1.0, 2.0, 1.0) } );

	std::cout << "col height: " << cast << ", player height: " << playerIn->playerPosition.y << std::endl;

	if (cast >= 0 && cast < 0.5) {
		//playerVelocity = glm::vec3(0.0);
		//std::cout << "colliding " << glfwGetTime() << std::endl;
	}

	playerIn->move(playerVelocity * deltaTime);

	// slow player down
	playerVelocity.x /= FRICTION;
	playerVelocity.z /= FRICTION;

	// round velocity to 0
	if (abs(playerVelocity.x) < 0.001)
		playerVelocity.x = 0;
	if (abs(playerVelocity.z) < 0.001)
		playerVelocity.z = 0;
}

void PlayerController::moveForward(Player* playerIn, float deltaTime)
{
	glm::vec3 vel = (vecFromYaw(playerIn->playerRotation.y));
	playerVelocity.x += vel.x;
	playerVelocity.z += vel.z;
}

void PlayerController::moveBackward(Player* playerIn, float deltaTime)
{
	glm::vec3 vel = -(vecFromYaw(playerIn->playerRotation.y));
	playerVelocity.x += vel.x;
	playerVelocity.z += vel.z;
}

void PlayerController::moveLeft(Player* playerIn, float deltaTime)
{
	glm::vec3 vel = vecFromYaw(playerIn->playerRotation.y - 90);
	playerVelocity.x += vel.x;
	playerVelocity.z += vel.z;
}

void PlayerController::moveRight(Player* playerIn, float deltaTime)
{
	glm::vec3 vel = vecFromYaw(playerIn->playerRotation.y + 90);
	playerVelocity.x += vel.x;
	playerVelocity.z += vel.z;
}