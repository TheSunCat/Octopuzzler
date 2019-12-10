#include "PlayerController.h"
#include "Constants.h"

void PlayerController::updatePlayer(Player* playerIn, float deltaTime)
{
	if (keyForward)
		moveForward(playerIn, deltaTime);
	if (keyBackward)
		moveBackward(playerIn, deltaTime);
	if (keyLeft)
		moveLeft(playerIn, deltaTime);
	if (keyRight)
		moveRight(playerIn, deltaTime);

	if (keyJump && canJump)
		playerVelocity = glm::vec3(0, 5, 0);
}

void PlayerController::updatePhysics(Player* playerIn, float deltaTime)
{
	if (playerIn->playerPosition.y <= (0.0))
		canJump = true;
	else
		canJump = false;

	playerIn->move(playerVelocity * deltaTime);

	// gravity
	playerVelocity += glm::vec3(0, GRAVITY, 0);

	if (canJump)
		playerVelocity = glm::vec3(0.0);
}

void PlayerController::moveForward(Player* playerIn, float deltaTime)
{
	playerIn->move(vecFromYaw(playerIn->playerRotation.y) * (deltaTime * 5));
}

void PlayerController::moveBackward(Player* playerIn, float deltaTime)
{
	playerIn->move(-(vecFromYaw(playerIn->playerRotation.y) * (deltaTime * 5)));
}

void PlayerController::moveLeft(Player* playerIn, float deltaTime)
{
	playerIn->move(vecFromYaw(playerIn->playerRotation.y - 90) * (deltaTime * 5));
}

void PlayerController::moveRight(Player* playerIn, float deltaTime)
{
	playerIn->move(vecFromYaw(playerIn->playerRotation.y + 90) * (deltaTime * 5));
}