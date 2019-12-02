#include "PlayerController.h"

void PlayerController::updatePlayer(Player* playerIn)
{
	if (keyForward)
		moveForward(playerIn);
	if (keyBackward)
		moveBackward(playerIn);
	if (keyLeft)
		moveLeft(playerIn);
	if (keyRight)
		moveRight(playerIn);
}

void PlayerController::moveForward(Player* playerIn)
{
	playerIn->move(vecFromYaw(playerIn->playerRotation.y));
}

void PlayerController::moveBackward(Player* playerIn)
{
	playerIn->move(-vecFromYaw(playerIn->playerRotation.y));
}

void PlayerController::moveLeft(Player* playerIn)
{
	playerIn->move(vecFromYaw(playerIn->playerRotation.y - 90));
}

void PlayerController::moveRight(Player* playerIn)
{
	playerIn->move(vecFromYaw(playerIn->playerRotation.y + 90));
}