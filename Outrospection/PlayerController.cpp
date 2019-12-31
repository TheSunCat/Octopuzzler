#include "PlayerController.h"
#include "Constants.h"

void PlayerController::updatePlayer(Player* playerIn, float deltaTime)
{
	if (keyJump && isGrounded) {
		playerVelocity.y = -GRAVITY * 30;
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
	// gravity
	if(playerVelocity.y > GRAVITY * 8) // terminal velocity is GRAVITY * 8
		playerVelocity += glm::vec3(0, GRAVITY, 0);

	// get the player's down-facing ray, for gravity
	Ray downRay = Ray{ playerIn->playerPosition, glm::vec3(0.0, -1.0, 0.0) };

	// temp hardcoded collision data because filetype is buggy
	std::vector<Triangle> colData = {
		Triangle { glm::vec3(10.0, 0.0, 10.0), glm::vec3(10.0, 0.0, -10.0), glm::vec3(-10.0, 0.0, 0.0) }
	};

	isGrounded = false; // always assume player isn't grounded until we know.

	for (Triangle curTri : colData) {
		float cast = rayCast(downRay, curTri);

		if (!isGrounded)
			isGrounded = cast != -INFINITY && cast <= 0.05; // if cast is -INFINITY, it means the ray doesn't collide with the tri ever
															// if cast is close to zero, however, it means we're pretty much touching it

		if (cast == -INFINITY)
			continue;


		// this is so we don't fall thru floors
		// castGravSum will be negative if applying gravity will cause us to phase thru the floor like a ghost, spooky
		float castGravSum = cast + GRAVITY;

		if (castGravSum < 0 && playerVelocity.y < 0) { // if we'd go thru the floor *and* we're moving down

			playerVelocity.y = -cast * 10;
		}
	}

	std::cout << " grounded: " << isGrounded << std::endl;

	playerVelocity *= deltaTime;

	playerIn->move(playerVelocity);

	playerVelocity /= deltaTime;

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