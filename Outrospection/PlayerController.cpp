#include "PlayerController.h"
#include "Constants.h"

void PlayerController::updatePlayer(Player* playerIn, float deltaTime)
{
	if (keyJump && canJump)
		playerVelocity = glm::vec3(0, 5, 0);

	if (keyForward)
		moveForward(playerIn, deltaTime);
	if (keyBackward)
		moveBackward(playerIn, deltaTime);
	if (keyLeft)
		moveLeft(playerIn, deltaTime);
	if (keyRight)
		moveRight(playerIn, deltaTime);
}

void PlayerController::updatePhysics(Player* playerIn, float deltaTime)
{
	playerIn->move(playerVelocity * deltaTime);

	if (playerIn->playerPosition.y <= (0.0))
		canJump = true;
	else
		canJump = false;

	// gravity
	playerVelocity += glm::vec3(0, GRAVITY, 0);

	if (canJump)
		playerVelocity.y = 0.0;
}

void PlayerController::moveForward(Player* playerIn, float deltaTime)
{
	playerVelocity += (vecFromYaw(playerIn->playerRotation.y) * 5.0f) * (deltaTime);
}

void PlayerController::moveBackward(Player* playerIn, float deltaTime)
{
	playerVelocity += -(vecFromYaw(playerIn->playerRotation.y) * 5.0f) * deltaTime;
}

void PlayerController::moveLeft(Player* playerIn, float deltaTime)
{
	playerVelocity += vecFromYaw(playerIn->playerRotation.y - 90) * 5.0f * deltaTime;
}

void PlayerController::moveRight(Player* playerIn, float deltaTime)
{
	playerVelocity += vecFromYaw(playerIn->playerRotation.y + 90) * 5.0f * (deltaTime);
}