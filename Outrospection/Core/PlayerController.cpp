#include "PlayerController.h"

#include <iostream>

#include "Util.h"
#include "Constants.h"
#include "Controller.h"
#include "Source.h"

#include "Core/Player.h"

void PlayerController::acceleratePlayer(Player* playerIn, const Controller& controller, float deltaTime)
{
	const glm::vec3 inputMoveVector = processInput(controller, playerIn->playerRotation.y);
	playerVelocity += inputMoveVector * 3.0f;

	// apply gravity
	playerVelocity.y += GRAVITY * deltaTime;
	
	if (controller.jump)
	{
		if (grounded || DEBUG && controller.talk) // cheat code hold TALK to moonjump
		{
			jumping = true;
			playerVelocity.y = 0.5f;
		}
	}
	
	// apply friction
	playerVelocity.x *= FRICTION * deltaTime;
	playerVelocity.z *= FRICTION * deltaTime;

	// round low velocity to 0
	if (abs(playerVelocity.x) < 0.001)
		playerVelocity.x = 0;
	if (abs(playerVelocity.z) < 0.001)
		playerVelocity.z = 0;

	// clamp velocity
	//if (playerVelocity.y > 1.5f)
	//	playerVelocity.y = 1.5f;

	if (playerVelocity.y < -0.5f) // 1.0 is the terminal velocity
		playerVelocity.y = -0.5f;

	std::cout << Util::vecToStr(playerVelocity) << std::endl;
}

void PlayerController::collidePlayer(Player* playerIn, const std::vector<Triangle>& collisionData)
{
	// reset important fields
	colResponseDelta = glm::vec3(0);
	grounded = false;
	
	if (Util::isZeroV3(playerVelocity))
		return;

	resolveCollision(playerIn, collisionData);
}

// return true when another iteration must be run to resolve all collision
void PlayerController::resolveCollision(Player* playerIn, const std::vector<Triangle>& collisionData)
{
	if(getOutrospection()->controller.debugBreak)
	{
		std::cout << "DEBUG: break" << std::endl;
	}

	colResponseDelta = glm::vec3(0.0f);

	std::vector<Triangle> groundCollisions;
	std::vector<Collision> wallCollisions;

	const float colSphereRadius = 0.35f;
	const float colSphereRadiusSquare = colSphereRadius * colSphereRadius;

	const glm::vec3 playerColliderOrigin = playerIn->playerPosition + glm::vec3(0, colSphereRadius, 0);
	
	glm::vec3 playerGhostPosition = playerColliderOrigin + playerVelocity;
	float playerGhostDistance = glm::length(playerVelocity); // TODO prevent sqrt here
	
	const glm::vec3 playerDirection = glm::normalize(playerVelocity);

	//const Ray playerRay{ playerColliderOrigin, playerDirection };
	//const Collision playerRayHit = Util::rayCast(playerRay, collisionData);
	
	const glm::vec3 colSphereOrigin = /*playerRayHit.dist < playerGhostDistance
		                                   ? playerRayHit.point
		                                   :*/ playerGhostPosition; // there's a tri we would skip over, so instead tp player to here

	//if(playerRayHit.dist < playerGhostDistance) // there's a try we would skip over
	//{
	//	playerVelocity = playerDirection * (playerRayHit.dist - colSphereRadius);

	//	// recalculate these 
	//	playerGhostPosition = playerColliderOrigin + playerVelocity;
	//	playerGhostDistance = glm::length(playerVelocity);
	//	
	//}
	
    int numCollisions = 0;
	
    for (const Triangle& curTri : collisionData)
    {
        //bool outsidePlane = false;
        bool outsideAllVerts = false;
        bool outsideAllEdges = false;
        bool fullyInsidePlane = false;

        glm::vec3 v0 = curTri.v0;
        glm::vec3 v1 = curTri.v1;
        glm::vec3 v2 = curTri.v2;

        float d = -glm::dot((v0 + v1 + v2) / 3.0f, curTri.n);

        // the distance from the center of the collider sphere to the triangle
        float pointToPlaneDist = glm::dot(curTri.n, colSphereOrigin) + d;

    	
        if (fabs(pointToPlaneDist) > colSphereRadius)
        {
            // distance from center to plane is larger than sphere radius
            continue;
        }

        // build 3 rays (line segments) so we can do plane projection later
        glm::vec3 v1v0 = v1 - v0;
        glm::vec3 v2v1 = v2 - v1;
        glm::vec3 v0v2 = v0 - v2;

        // project to triangle plane (3D -> 2D) and see if we are within its bounds
        glm::vec3 planeX = glm::normalize(v1v0);
        glm::vec3 planeY = glm::normalize(glm::cross(curTri.n, v1v0));

        // local function to do projection
        auto project2D = [&](const glm::vec3& p) { return glm::vec2(glm::dot(p, planeX), glm::dot(p, planeY)); };

        glm::vec2 planePos2D = project2D(colSphereOrigin);
        glm::vec2 triangle2D[3] = { project2D(v0), project2D(v1), project2D(v2) };

        if (Util::pointInside(triangle2D, 3, planePos2D))
        {
            fullyInsidePlane = true;
        }
		

    	// check vertices
        bool outsideV1 = (Util::length2V3(v0 - colSphereOrigin) > colSphereRadiusSquare);
        bool outsideV2 = (Util::length2V3(v1 - colSphereOrigin) > colSphereRadiusSquare);
        bool outsideV3 = (Util::length2V3(v2 - colSphereOrigin) > colSphereRadiusSquare);

        if (outsideV1 && outsideV2 && outsideV3)
        {
            //sphere outside of of all triangle vertices
            outsideAllVerts = true;
        }


    	// check lines (rays)
        glm::vec3 intersectPoint;

		if (!Util::intersectRaySegmentSphere(Ray{ v0, v1v0 }, colSphereOrigin, colSphereRadiusSquare, intersectPoint) &&
			!Util::intersectRaySegmentSphere(Ray{ v1, v2v1 }, colSphereOrigin, colSphereRadiusSquare, intersectPoint) &&
            !Util::intersectRaySegmentSphere(Ray{ v2, v0v2 }, colSphereOrigin, colSphereRadiusSquare, intersectPoint))
        {
            //sphere outside of all triangle edges
            outsideAllEdges = true;
        }

        if (outsideAllVerts && outsideAllEdges && !fullyInsidePlane)
        {
            continue;
        }

    	
    	// --------------------------------------------------
    	// we found a collision!
        // push the character outside of the triangle we intersected

		if (fabs(curTri.n.y) > 0.1) // we are colliding with ground
		{
			groundCollisions.emplace_back(curTri);

			grounded = true;
		}
		else // handle wall collisions
    	{
			break;
			
			// we need to correct by this
 			const float distancePastTri = playerGhostDistance - (glm::distance(playerIn->playerPosition, intersectPoint));

			// angle at which we run into
			const float cosine = fabs(Util::cosBetweenV3(playerDirection, curTri.n));

			std::cout << pointToPlaneDist << std::endl;

			const float hypotenuse = distancePastTri, adjacentSide = cosine * hypotenuse;
			const float oppositeSide = sqrt(pow(hypotenuse, 2) - pow(adjacentSide, 2));
    		
			glm::vec3 curCollisionShift = curTri.n * (colSphereRadius + oppositeSide);

			colResponseDelta += curCollisionShift;

			Collision col{ pointToPlaneDist, intersectPoint, curTri, curCollisionShift };
			wallCollisions.emplace_back(col);
		}

		numCollisions++;
    }

    if (numCollisions != 0 || !groundCollisions.empty())
    {
    	if(numCollisions > 0)
			colResponseDelta /= float(numCollisions); // average our collision responses?


		float highestY = -INFINITY;
    	// do this after ALL mods to playerVelocity and colResponseDelta are done
		for (const Triangle& curTri : groundCollisions)
		{
			glm::vec3 newPlayerPos = playerIn->playerPosition + playerVelocity + colResponseDelta;

			float y = ((curTri.n.x * (newPlayerPos.x - curTri.v0.x)
				+ curTri.n.z * (newPlayerPos.z - curTri.v0.z)) / -curTri.n.y)
				+ curTri.v0.y;

			y = y - playerIn->playerPosition.y;

			if (y > highestY)
				highestY = y;
		}

    	for(const Collision& collision : wallCollisions)
    	{
			if (collision.point.y <= highestY) // we're colliding with stuff under the ground we're on
			{
				//colResponseDelta -= collision.shiftBy;
			}
    	}

		if (highestY != -INFINITY)
		{
			colResponseDelta.y = highestY;
			playerVelocity.y = 0;
		}
    }
}

// TODO implement this function
// ReSharper disable once CppMemberFunctionMayBeStatic
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

void PlayerController::movePlayer(Player* playerIn) const
{
	//if(double(rand()) / RAND_MAX < 0.2)
	//{
	//	for (int i = 0; i < 30000000; i++);
	//}
	
	if (playerVelocity.x != playerVelocity.x)
		std::cout << "ERROR: playerVelocity is " << Util::vecToStr(playerVelocity) << std::endl;
	
	playerIn->move(playerVelocity + colResponseDelta);

	if (playerIn->playerPosition.x != playerIn->playerPosition.x)
		std::cout << "ERROR: playerPosition is " << Util::vecToStr(playerIn->playerPosition) << std::endl;
	
	//std::cout << Util::vecToStr(playerIn->playerPosition) << std::endl;
}

glm::vec3 PlayerController::processInput(const Controller& controller, const float yaw)
{
	glm::vec3 inputVector(0.0f);
	inputVector += Util::rotToVec3(yaw) * controller.leftForward;
	inputVector += Util::rotToVec3(yaw + 90) * controller.leftSide;

	if (Util::length2V3(inputVector) > 1.0) { // normalize if we would move too fast
		inputVector = glm::normalize(inputVector);
	}

	return inputVector;
}
