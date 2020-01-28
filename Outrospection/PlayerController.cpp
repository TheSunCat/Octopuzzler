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
		playerInputAcceleration = glm::normalize(playerInputAcceleration);

		playerInputAcceleration /= 75;

		playerVelocity += playerInputAcceleration;
	}

	if (keyJump) {
		if (isGrounded) {
			playerVelocity.y = 0.05;
		}
	}

	if(playerVelocity.y > GRAVITY * 200) // GRAVITY * 200 is the terminal velocity
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
		if (keyAttack) {
			playerIn->playerPosition = glm::vec3(0, 0, 0);
			playerVelocity = glm::vec3(0.0f);
		}
	}
}

void PlayerController::collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime)
{
	deltaTime *= 100;

	if (isZero3(playerVelocity))
		return;

	playerVelocity *= deltaTime;

	Ray downRay = Ray{ playerIn->playerPosition, glm::vec3(0, -1, 0) };

	// detect if player is on ground
	// -----------------------------

	isGrounded = false; // assume player isn't on ground until we know

	for (Triangle curTri : collisionData) {
		RayHit groundHit = rayCast(downRay, curTri, false);
		if (!std::isnan(groundHit.dist) && groundHit.dist < 0.05) { // if the ground exists under player, and is close to player's feet (0.05)
			isGrounded = true;
			break; // only need to check *if* we're on ground, only once
		}
	}

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

		if (collisionCount > 10) {
			playerVelocity = glm::vec3(0.0);
			return; // prevent infinite loops (?)
		}
	}

	playerVelocity /= deltaTime;
}

// return true when another iteration must be run to resolve all collision
bool PlayerController::resolveCollision(Player* playerIn, const std::vector<Triangle>& collisionData)
{
	if (playerVelocity == glm::vec3(0.0))
		return false;

	glm::vec3 colCalcOffset = glm::vec3(0.0, 0.0, 0.0);
	Ray playerRay = Ray{ playerIn->playerPosition + colCalcOffset, normalize(playerVelocity) };
	Ray playerHeadRay = Ray{ playerIn->playerPosition + glm::vec3(0, 0.5, 0), normalize(playerVelocity) };
	float playerVelMagnitude = glm::length(playerVelocity);

	// get RayHit of closest wall
	RayHit hit = cast(playerRay, collisionData);
	RayHit topHit = cast(playerHeadRay, collisionData);

	// if there is a triangle within the dist we will move next frame
	if (!std::isnan(hit.dist) && hit.dist < playerVelMagnitude) {

		glm::vec3 calcPos = playerIn->playerPosition + colCalcOffset;

		// position if player *were* a ghost and *could* go thru walls (warning: spooky)
		glm::vec3 ghostPosition = calcPos + playerVelocity;

		// ray starting from ghost pos back to tri
		Ray ghostRay = { ghostPosition, hit.tri.n };

		// where ghostRay intersects the tri's plane would be where the player would end up since she's not a ghost
		glm::vec3 notGhostPosition = rayCastPlane(ghostRay, hit.tri);

		notGhostPosition += ghostRay.direction * 0.001f;

		// if the ray does not collide with the plane, something's wrong
		if (std::isnan(notGhostPosition.x))
			playerVelocity = ghostPosition - calcPos;
		else
			playerVelocity = (notGhostPosition - calcPos + (hit.tri.n * 0.001f)); // adding normal "skin offset" so player can go up slopes

		// e
		if (topHit.dist < hit.dist || (std::isnan(hit.dist) && !std::isnan(topHit.dist))) {
			colCalcOffset.y = 0.5;


			if (topHit.dist < playerVelMagnitude) {
				// raycast down from head to toes and see if there's any impaling tris. if so, BAD.
				Ray checkRay = Ray{ playerIn->playerPosition + colCalcOffset /*+ playerVelocity*/, glm::vec3(0, -1, 0) };
				RayHit checkHit = cast(checkRay, collisionData);

				if (checkHit.dist < colCalcOffset.y) { // cancel all movement if player would be impaled. TODO maybe slide? keeping it simple.
					//playerIn->move(-glm::vec3(playerVelocity.x, 0.0f, playerVelocity.z) * 1.0f);
					playerVelocity = glm::vec3(0.0);
					playerIn->playerPosition = lastGoodPlayerPosition;
					return false;
				}
			}

			hit = topHit;
		}
		return true;
	}

	if (topHit.dist < hit.dist || (std::isnan(hit.dist) && !std::isnan(topHit.dist))) {
		colCalcOffset.y = 0.5;


		if (topHit.dist < playerVelMagnitude) {
			// raycast down from head to toes and see if there's any impaling tris. if so, BAD.
			Ray checkRay = Ray{ playerIn->playerPosition + colCalcOffset /*+ playerVelocity*/, glm::vec3(0, -1, 0) };
			RayHit checkHit = cast(checkRay, collisionData);

			if (checkHit.dist < colCalcOffset.y) { // cancel all movement if player would be impaled. TODO maybe slide? keeping it simple.
				//playerIn->move(-playerVelocity * 1.0f);
				playerVelocity = glm::vec3(0.0);
				playerIn->playerPosition = lastGoodPlayerPosition;
				return false;
			}
		}

		hit = topHit;
	}

	// nothing was hit
	return false;
}

RayHit PlayerController::cast(Ray r, const std::vector<Triangle>& collisionData)
{
	RayHit closestHit = RayHit { INFINITY };

	for (Triangle tri : collisionData) {
		RayHit hit = rayCast(r, tri, false);

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
	AnimType newAnim;

	if (isGrounded && length2(playerVelocity) < 0.0000002) {
		newAnim = AnimType::idle;
	}
	else if (!isGrounded) {
		if (playerVelocity.y > -0.01) {
			newAnim = AnimType::jump;
		}
		else {
			newAnim = AnimType::fall;
		}
	}
	else {
		newAnim = AnimType::walk;
	}

	if (newAnim != pastAnim)
		playerIn->setAnimation(newAnim);

	pastAnim = newAnim;
}

void PlayerController::movePlayer(Player* playerIn, float deltaTime)
{
	deltaTime *= 100;

	playerVelocity *= deltaTime;

	if (std::isnan(playerVelocity.x) || std::isnan(playerVelocity.y) || std::isnan(playerVelocity.z))
		std::cout << "ERROR: playerVelocity is " << vecToStr(playerVelocity) << std::endl;

	lastGoodPlayerPosition = playerIn->playerPosition;

	playerIn->move(playerVelocity);

	playerVelocity /= deltaTime;

	std::cout << vecToStr(playerIn->playerPosition) << std::endl;
}