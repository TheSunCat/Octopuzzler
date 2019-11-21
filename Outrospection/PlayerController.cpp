#include "PlayerController.h"

void PlayerController::updatePlayer(Player playerIn)
{
	if (keyForward)
		moveForward(playerIn);
	if (keyBackward)
		moveBackward(playerIn);
}

void PlayerController::moveForward(Player playerIn)
{
	playerIn.move(vecFromYaw(playerIn.rotation.y));
}

void PlayerController::moveBackward(Player playerIn)
{
	playerIn.move(-vecFromYaw(playerIn.rotation.y));
}