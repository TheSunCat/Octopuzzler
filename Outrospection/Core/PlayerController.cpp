#include "PlayerController.h"

#include <iostream>

#include "Util.h"
#include "Constants.h"
#include "Controller.h"

#include "Core/Player.h"

void PlayerController::acceleratePlayer(Player* playerIn, const Controller& controller)
{
	glm::vec3 inputMoveVector(0.0f);

	inputMoveVector += Util::rotToVec3(playerIn->playerRotation.y) * controller.leftForward;
	inputMoveVector += Util::rotToVec3(playerIn->playerRotation.y + 90) * controller.leftSide;

	if (Util::length2V3(inputMoveVector) > 1.0) { // normalize if we would move too fast
		inputMoveVector = glm::normalize(inputMoveVector);
	}

	playerVelocity += inputMoveVector;

	if (controller.jump)
	{
		if (grounded || DEBUG && controller.talk) // cheat code hold TALK to moonjump
		{
			jumping = true;
			playerVelocity.y = 10;
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
}

void PlayerController::collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData, float deltaTime)
{
	velocityShift = glm::vec3(0);
	
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

	for (const Triangle& curTri : collisionData)
	{
		RayHit groundHit = Util::rayCast(downRay, curTri, false);
		if (groundHit.dist != INFINITY && groundHit.dist < 0.05) // if the ground exists under player, and is close to player's feet (0.05)
		{
			grounded = true;
			break; // only need to check *if* we're on ground, only once
		}
	}


	playerVelocity /= deltaTime;
}

// return true when another iteration must be run to resolve all collision
bool PlayerController::resolveCollision(Player* playerIn, const std::vector<Triangle>& collisionData)
{
	if (playerVelocity == glm::vec3(0.0))
		return false;

    const glm::vec3 collSphereOrigin = playerIn->playerPosition + glm::vec3(0, 0.3f, 0) + playerVelocity;
    const float colSphereRadius = 0.3f;
    const float colSphereRadiusSquare = colSphereRadius * colSphereRadius;

    int numCollisions = 0;

    //for each triangle in the collision geometry
    for (const Triangle& curTri : collisionData)
    {
        //bool outsidePlane = false;
        bool outsideAllVerts = false;
        bool outsideAllEdges = false;
        bool fullyInsidePlane = false;

        glm::vec3 v0 = curTri.v0;
        glm::vec3 v1 = curTri.v1;
        glm::vec3 v2 = curTri.v2;

        // assume flat normals for collision (all 3 n would be the same)
        glm::vec3 pN = curTri.n;

        // only test vertical polygons
        //if (fabs(pN.y) > 0.1f)
        //    continue;

        float d = -glm::dot((v0 + v1 + v2) / 3.0f, pN);

        // get point-to-plane distance from model center
        float pointToPlaneDist = glm::dot(pN, collSphereOrigin) + d;

    	
        if (fabs(pointToPlaneDist) > colSphereRadius)
        {
            // sphere outside of infinite triangle plane
            continue;
        }

        // build 3 rays (line segments) so we can do plane projection later
        glm::vec3 v1v0 = v1 - v0;
        glm::vec3 v2v1 = v2 - v1;
        glm::vec3 v0v2 = v0 - v2;

        // project to triangle plane (3D -> 2D) and see if we are within its bounds
        glm::vec3 planeX = glm::normalize(v1v0);
        glm::vec3 planeY = glm::normalize(glm::cross(pN, v1v0));

        // local function to do projection
        auto project2D = [&](const glm::vec3& p) { return glm::vec2(glm::dot(p, planeX), glm::dot(p, planeY)); };

        glm::vec2 planePos2D = project2D(collSphereOrigin);
        glm::vec2 triangle2D[3] = { project2D(v0), project2D(v1), project2D(v2) };

        if (Util::pointInside(triangle2D, 3, planePos2D))
        {
            fullyInsidePlane = true;
        }
		

    	// check vertices
        bool outsideV1 = (Util::length2V3(v0 - collSphereOrigin) > colSphereRadiusSquare);
        bool outsideV2 = (Util::length2V3(v1 - collSphereOrigin) > colSphereRadiusSquare);
        bool outsideV3 = (Util::length2V3(v2 - collSphereOrigin) > colSphereRadiusSquare);

        if (outsideV1 && outsideV2 && outsideV3)
        {
            //sphere outside of of all triangle vertices
            outsideAllVerts = true;
        }


    	// check lines (rays)
        glm::vec3 ip;

		if (!Util::intersectRaySegmentSphere(Ray{ v0, v1v0 }, collSphereOrigin, colSphereRadiusSquare, ip) &&
			!Util::intersectRaySegmentSphere(Ray{ v1, v2v1 }, collSphereOrigin, colSphereRadiusSquare, ip) &&
            !Util::intersectRaySegmentSphere(Ray{ v2, v0v2 }, collSphereOrigin, colSphereRadiusSquare, ip))
        {
            //sphere outside of all triangle edges
            outsideAllEdges = true;
        }

        if (outsideAllVerts && outsideAllEdges && !fullyInsidePlane)
        {
            continue;
        }

    	
    	// we have a collision!
    	// 
        // push the character outside of the intersected body

		//if(fabs(curTri.n.y) > 0.1) // this is not a wall!
		{
			//velocityShift.y = collSphereOrigin.y - ip.y;
			
			//playerVelocity.y = 0;
		}
    	//else
		{
			// go through the tri, cast a ray towards tri normal
			// to see where the player should slide to
			/*Ray ghostRay = Ray{ collSphereOrigin + playerVelocity , curTri.n };
			glm::vec3 hitPos = Util::rayCastPlane(ghostRay, curTri);
			hitPos += curTri.n * colSphereRadius;*/


			velocityShift += pN * (colSphereRadius - pointToPlaneDist);

        	if(fabs(velocityShift.y - playerVelocity.y) < 0.1)
        	{
				velocityShift.y = 0;
				playerVelocity.y = 0;
        	}
		}
    	
    	
        numCollisions++;
    }

    if (numCollisions != 0)
    {
        velocityShift /= float(numCollisions);

        //if (shiftDelta.length() > lastWalkSpeed)
        //{
        //    shiftDelta = glm::normalize(shiftDelta);
        //    shiftDelta *= lastWalkSpeed * 1.1f;
        //}
    }

	return false; // does not need more solving?
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

	//lastGoodPlayerPosition = playerIn->playerPosition - playerVelocity;

	playerIn->move(playerVelocity * deltaTime + velocityShift);

	std::cout << Util::vecToStr(playerIn->playerPosition) << ", " << Util::vecToStr(playerIn->playerCharacter.charPosition) << std::endl;
}