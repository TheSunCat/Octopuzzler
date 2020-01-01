#include "PlayerController.h"
#include "Constants.h"

void PlayerController::acceleratePlayer(Player* playerIn)
{
	if (keyForward) {
		playerVelocity += (vecFromYaw(playerIn->playerRotation.y));
	}

	if (keyBackward) {
		playerVelocity += -(vecFromYaw(playerIn->playerRotation.y));
	}

	if (keyLeft) {
		playerVelocity += vecFromYaw(playerIn->playerRotation.y - 90);
	}

	if (keyRight) {
		playerVelocity += vecFromYaw(playerIn->playerRotation.y + 90);
	}

	//if(playerVelocity.y > GRAVITY * 8) // GRAVITY * 8 is terminal velocity
	//	playerVelocity.y += GRAVITY;

	// slow player down
	playerVelocity.x /= FRICTION;
	playerVelocity.z /= FRICTION;

	// round velocity to 0
	if (abs(playerVelocity.x) < 0.001)
		playerVelocity.x = 0;
	if (abs(playerVelocity.z) < 0.001)
		playerVelocity.z = 0;

	playerVelocity /= 10; // slow down bc no deltatime
	
	// DEBUG move up and down
	if (keyAttack) {
		playerIn->move(glm::vec3(0,  GRAVITY / 4, 0));
	}

	if (keyJump) {
		playerIn->move(glm::vec3(0, -GRAVITY / 4, 0));
	}
}

void PlayerController::collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime)
{
	if (playerVelocity == glm::vec3(0.0))
		return;

	Ray playerRay = Ray{ playerIn->playerPosition, normalize(playerVelocity) };

	RayHit closest;

	float playerVelMagnitude = glm::length(playerVelocity);

	//Triangle t = Triangle{ glm::vec3(10, 0, 10), glm::vec3(10, 0, -10), glm::vec3(10, 10, 0) };
	//t.n = getNormal(t);

	//std::vector<Triangle> colData = {
	//	t
	//};

	for (Triangle curTri : collisionData)
	{
		RayHit curHit = rayCast(playerRay, curTri);

		if (curHit.dist == -INFINITY)
			continue; // not hitting tri

		if (curHit.dist < playerVelMagnitude) {
			glm::vec3 normalizedPlayerVel = normalize(playerVelocity);
			playerVelMagnitude = glm::length(playerVelocity);

			glm::vec3 normalizedSlideVel = normalizedPlayerVel + curTri.n;

			playerVelocity = normalizedSlideVel * playerVelMagnitude;
		}
	}
}

void PlayerController::movePlayer(Player* playerIn)
{
	std::cout << vecToStr(playerIn->playerPosition) << std::endl;

	playerIn->move(playerVelocity);
}