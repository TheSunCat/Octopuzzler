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

	// debug hardcoded collision data
	Triangle t = Triangle{ glm::vec3(5, 0, 10), glm::vec3(5, 0, -10), glm::vec3(5, 10, 0) };
	t.n = getNormal(t);

	Triangle t1 = Triangle{ glm::vec3(5.5, 0, 10), glm::vec3(5.5, 0, -10), glm::vec3(5.5, 10, 0) };
	t1.n = getNormal(t1);

	Triangle t2 = Triangle{ glm::vec3(10, 0, 10), glm::vec3(10, 0, -10), glm::vec3(-10, 0, 0) };
	t2.n = getNormal(t2);

	std::vector<Triangle> colData = {
		t, t1, t2
	};

	Ray playerRay = Ray{ playerIn->playerPosition, normalize(playerVelocity) };
	Ray downRay = Ray{ playerIn->playerPosition, glm::vec3(0, -1, 0) };

	RayHit closest;

	float playerVelMagnitude = glm::length(playerVelocity);

	for (Triangle curTri : colData)
	{
		// wall collision
		RayHit curHit = rayCast(playerRay, curTri);

		if (curHit.dist != -INFINITY) {
			if (curHit.dist < playerVelMagnitude) {
				glm::vec3 ghostPosition = playerIn->playerPosition + playerVelocity;

				Ray ghostRay = { ghostPosition, curTri.n };

				glm::vec3 notGhostPosition = rayCastPlane(ghostRay, curTri);

				playerVelocity = (notGhostPosition - playerIn->playerPosition) + (curTri.n * 0.01f);
			}
		}

		// ground collision
		RayHit downHit = rayCast(downRay, curTri);

		if (downHit.dist != -INFINITY) {
			if (downHit.dist < -playerVelocity.y) {
				std::cout << "grounded ";
			}
		}
	}
}

void PlayerController::movePlayer(Player* playerIn)
{
	playerIn->move(playerVelocity);
	std::cout << vecToStr(playerIn->playerPosition) << std::endl;
}