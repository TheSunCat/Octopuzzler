#include "PlayerController.h"

#include "Util.h"
#include "Constants.h"

void PlayerController::acceleratePlayer(Player* playerIn, const Controller& controller)
{
	glm::vec3 inputMoveVector(0.0f);

	if (abs(controller.forward) > 0.1)
		inputMoveVector += Util::vecFromYaw(playerIn->playerRotation.y) * controller.forward;
	if (abs(controller.right) > 0.1)
		inputMoveVector += Util::vecFromYaw(playerIn->playerRotation.y + 90) * controller.right;

	if (!Util::isZeroV3(inputMoveVector)) { // avoid NaN normalization if no input is given
		inputMoveVector = glm::normalize(inputMoveVector);

		playerVelocity += inputMoveVector;
	}

	if (controller.jump) {
		if (grounded) {
			jumping = true;
			playerVelocity.y = 5;
		}
	}

	if(playerVelocity.y > GRAVITY * 20) // GRAVITY * 20 is the terminal velocity
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
	if (DEBUG) {
		//if (keyAttack) {
		//	playerIn->playerPosition = glm::vec3(0, 0, 0);
		//	playerVelocity = glm::vec3(0.0f);
		//}
	}
}

void PlayerController::collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime)
{
	if (Util::isZeroV3(playerVelocity))
		return;

	playerVelocity *= deltaTime;

	// collide with walls
	// ------------------

	unsigned int collisionCount = 0;

	while (resolveCollision(playerIn, collisionData)) {
		// Now that we've resolved collision, we need to check
		// if we'd be going thru other tris with this newly set velocity.

		//       \
		//        \o
		//         |\
		//      ---+\\-------- <-- this wall would be skipped, we need to check
		//         | \\
		// orig -> V  \V <- new ray

		collisionCount++;

		if (collisionCount > 20) {
			playerVelocity = glm::vec3(0.0);
			return; // prevent infinite loops (?)
		}
	}

	Ray downRay = Ray{ playerIn->playerPosition, glm::vec3(0, -1, 0) };

	// detect if player is on ground
	// -----------------------------

	grounded = false; // assume player isn't on ground until we know

	for (const Triangle& curTri : collisionData) {
		RayHit groundHit = Util::rayCast(downRay, curTri, false);
		if (!std::isnan(groundHit.dist) && groundHit.dist < 0.05) { // if the ground exists under player, and is close to player's feet (0.05)
			grounded = true;
			break; // only need to check *if* we're on ground, only once
		}
	}


	playerVelocity /= deltaTime;
}

// return true when another iteration must be run to resolve all collision
bool PlayerController::resolveCollision(Player* playerIn, const std::vector<Triangle>& collisionData)
{
	bool needsMoreSolving = false;

	if (playerVelocity == glm::vec3(0.0))
		return needsMoreSolving;

	glm::vec3 colCalcOffset = glm::vec3(0.0, 0.0, 0.0);

	Ray playerRay = Ray{ playerIn->playerPosition + colCalcOffset, normalize(playerVelocity) };
	Ray playerHeadRay = Ray{ playerIn->playerPosition + glm::vec3(0, 1.0, 0), normalize(playerVelocity) };
	float playerVelMagnitude = glm::length(playerVelocity);

	// get RayHit of closest wall
	RayHit hit = cast(playerRay, collisionData);
	RayHit topHit = cast(playerHeadRay, collisionData);

	if (topHit.dist < hit.dist || (std::isnan(hit.dist) && !std::isnan(topHit.dist))) {
		colCalcOffset.y = 1.0;

		hit = topHit;
	}

	// if there is a triangle within the dist we will move next frame
	if (!std::isnan(hit.dist) && hit.dist < playerVelMagnitude) {

		glm::vec3 calcPos = playerIn->playerPosition + colCalcOffset;

		// position if player *were* a ghost and *could* go thru walls (warning: spooky)
		glm::vec3 ghostPosition = calcPos + playerVelocity;

		// ray starting from ghost pos back to tri
		Ray ghostRay = { ghostPosition, hit.tri.n };

		// where ghostRay intersects the tri's plane would be where the player would end up since she's not a ghost
		glm::vec3 notGhostPosition = Util::rayCastPlane(ghostRay, hit.tri);

		if (std::isnan(notGhostPosition.x) || std::isnan(notGhostPosition.y) || std::isnan(notGhostPosition.z))
			std::cout << "ERROR: playerVelocity is " << Util::vecToStr(playerVelocity) << std::endl;

		notGhostPosition += hit.tri.n * 0.001f;

		// if the ray does not collide with the plane, something's wrong
		if (std::isnan(notGhostPosition.x))
			playerVelocity = ghostPosition - calcPos;
		else
			playerVelocity = (notGhostPosition - calcPos + (hit.tri.n * 0.001f)); // adding normal "skin offset" so player can go up slopes

		if (std::isnan(playerVelocity.x) || std::isnan(playerVelocity.y) || std::isnan(playerVelocity.z))
			std::cout << "ERROR: playerVelocity is " << Util::vecToStr(playerVelocity) << std::endl;

		// collision was detected, we need to check for MORE WALLS
		needsMoreSolving = true;
	}

	// Solve impaling.
	// raycast down from head to toes and see if there's any impaling tris. if so, BAD.

	Ray checkRay = Ray{ playerIn->playerPosition + glm::vec3(0, 1.0, 0) + playerVelocity, glm::vec3(0, -1, 0) }; // ghost position ray

	// raycast down from player head
	RayHit checkHit = cast(checkRay, collisionData);

	if (checkHit.dist < 1.0) { // cancel all movement if player would be impaled. TODO keep slide from past solving. this cur overrides all collision
		playerVelocity.x = 0.0f;
		playerVelocity.z = 0.0f;

		if (std::isnan(playerVelocity.x) || std::isnan(playerVelocity.y) || std::isnan(playerVelocity.z))
			std::cout << "ERROR: playerVelocity is " << Util::vecToStr(playerVelocity) << std::endl;

		return true;
	}

	return needsMoreSolving;
}

RayHit PlayerController::cast(Ray r, const std::vector<Triangle>& collisionData)
{
	RayHit closestHit = RayHit { INFINITY };

	for (const Triangle& tri : collisionData) {
		RayHit hit = Util::rayCast(r, tri, false);

		if (hit.dist < closestHit.dist) {
			closestHit = hit;
			closestHit.tri = tri;
		}
	}

	if (closestHit.dist == INFINITY)
		closestHit.dist = NAN;

	return closestHit;
}

void PlayerController::animatePlayer(Player* playerIn)
{
	return;

	//AnimType newAnim;

	//if (isGrounded && length2V3(playerVelocity) < 0.0000002) {
	//	newAnim = AnimType::idle;
	//}
	//else if (!isGrounded) {
	//	if (playerVelocity.y > -0.01) {
	//		newAnim = AnimType::jump;
	//	}
	//	else {
	//		newAnim = AnimType::fall;
	//	}
	//}
	//else {
	//	newAnim = AnimType::walk;
	//}

	//if (newAnim != pastAnim)
	//	playerIn->setAnimation(newAnim);

	//pastAnim = newAnim;
}

void PlayerController::movePlayer(Player* playerIn, float deltaTime)
{
	if (std::isnan(playerVelocity.x) || std::isnan(playerVelocity.y) || std::isnan(playerVelocity.z))
		std::cout << "ERROR: playerVelocity is " << Util::vecToStr(playerVelocity) << std::endl;

	lastGoodPlayerPosition = playerIn->playerPosition - playerVelocity;

	playerIn->move(playerVelocity * deltaTime);

	std::cout << Util::vecToStr(playerIn->playerPosition) << ", " << Util::vecToStr(playerIn->playerCharacter.charPosition) << std::endl;
}