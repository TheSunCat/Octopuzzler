#include "PlayerController.h"
#include "Constants.h"

void PlayerController::acceleratePlayer(Player* playerIn)
{
	glm::vec3 playerInputAcceleration(0.0f);

	if (keyForward) {
		playerInputAcceleration += (vecFromYaw(playerIn->playerRotation.y));
	}

	if (keyBackward) {
		playerInputAcceleration += -(vecFromYaw(playerIn->playerRotation.y));
	}

	if (keyLeft) {
		playerInputAcceleration += vecFromYaw(playerIn->playerRotation.y - 90);
	}

	if (keyRight) {
		playerInputAcceleration += vecFromYaw(playerIn->playerRotation.y + 90);
	}

	if (playerInputAcceleration != glm::vec3(0.0f)) { // avoid NaN normalization if no input is given
		glm::normalize(playerInputAcceleration);

		playerInputAcceleration /= 40;

		playerVelocity += playerInputAcceleration;

		if (isGrounded) {
			playerIn->setAnimation(AnimType::walk);
		}
	}

	bool jumping = false;
	if (keyJump) {
		if (isGrounded) {
			playerVelocity.y = 0.1;

			jumping = true;
			playerIn->setAnimation(AnimType::jump); // set animation to jumping
		}
	}

	if (!jumping && isGrounded)
		playerIn->setAnimation(AnimType::walk);
	if(playerVelocity.y > GRAVITY * 200) // GRAVITY * 2 is the terminal velocity
		playerVelocity.y += GRAVITY;

	// slow player down
	playerVelocity.x /= FRICTION;
	playerVelocity.z /= FRICTION;

	// round low velocity to 0
	if (abs(playerVelocity.x) < 0.001)
		playerVelocity.x = 0;
	if (abs(playerVelocity.z) < 0.001)
		playerVelocity.z = 0;

	// DEBUG move up and down
	if (keyAttack) {
		playerIn->move(glm::vec3(0, GRAVITY / 4, 0));
	}
}

void PlayerController::collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime)
{
	if (playerVelocity == glm::vec3(0.0f, 0.0f, 0.0f))
		return;

	// debug hardcoded collision data
	Triangle t = Triangle{ glm::vec3(5, 0, 10), glm::vec3(5, 0, -10), glm::vec3(10, 5, 0) };
	t.n = -getNormal(t);
	
	Triangle t1 = Triangle{ glm::vec3(10, 0, 10), glm::vec3(10, 0, -10), glm::vec3(-10, 0, 0) };
	t1.n = getNormal(t1);

	//Triangle t2 = Triangle{ glm::vec3(10, 0, 10), glm::vec3(10, 0, -10), glm::vec3(-10, 0, 0) };
	//t2.n = getNormal(t2);

	std::vector<Triangle> colData = {
		t, t1//, t2
	};

	Ray playerRay = Ray{ playerIn->playerPosition, normalize(playerVelocity) };
	Ray downRay = Ray{ playerIn->playerPosition, glm::vec3(0, -1, 0) };

	float playerVelMagnitude = glm::length(playerVelocity);

	bool groundChecked = false;

	for (Triangle curTri : collisionData)
	{
		// ground collision
		if (!groundChecked) {
			RayHit groundHit = rayCast(downRay, curTri, false);
			if (groundHit.dist != -INFINITY && groundHit.dist < 0.05) {
				isGrounded = true;
 				groundChecked = true;
			}
			else {
				isGrounded = false;
			}
		}
		// wall collision
		RayHit curHit = rayCast(playerRay, curTri, false);
		if (curHit.dist != -INFINITY && curHit.dist < playerVelMagnitude) {
			std::cout << "col" << std::endl;

			glm::vec3 ghostPosition = playerIn->playerPosition + playerVelocity;

			Ray ghostRay = { ghostPosition, curTri.n };

			glm::vec3 notGhostPosition = rayCastPlane(ghostRay, curTri);

			if (std::isnan(notGhostPosition.x))
				playerVelocity = ghostPosition - playerIn->playerPosition;
			else
				playerVelocity = (notGhostPosition - playerIn->playerPosition) + (curTri.n * 0.01f);

			if (playerVelocity != glm::vec3(0.0f, 0.0f, 0.0f)) {
				// update values and re-check collision for new velocity
				playerVelMagnitude = glm::length(playerVelocity);
				playerRay.direction = normalize(playerVelocity);

				for (Triangle t : collisionData) {
					RayHit newHit = rayCast(playerRay, t, false);

					if (newHit.dist != -INFINITY) {
						if (newHit.dist < playerVelMagnitude) {
							std::cout << "correcting for ghosting through wall when sliding..." << std::endl;
							playerVelocity = normalize(playerVelocity) * (newHit.dist * 0.9f);// - (curTri.n * 0.01f);
						}
					}
				}
			}
			else {
				break; // nothing else to check if we're not moving anymore
			}
		}
	}
}

void PlayerController::movePlayer(Player* playerIn)
{
	if (std::isnan(playerVelocity.x) || std::isnan(playerVelocity.y) || std::isnan(playerVelocity.z))
		std::cout << "ERROR: playerVelocity is " << vecToStr(playerVelocity) << std::endl;

	playerIn->move(playerVelocity);
	std::cout << vecToStr(playerIn->playerPosition) << std::endl;
}