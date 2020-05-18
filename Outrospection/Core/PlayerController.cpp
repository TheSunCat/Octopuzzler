#include "PlayerController.h"

#include <iostream>

#include "Util.h"
#include "Constants.h"
#include "Controller.h"
#include "Source.h"

#include "Core/Player.h"

void PlayerController::acceleratePlayer(const Player& player, const Controller& controller, const float deltaTime)
{
	const bool hacking = DEBUG && controller.leftTrigger >= 0.85f; // apparently, hacking is a side effect of debug mode - smug

	const glm::vec3 inputMoveVector = processInput(controller, player.yaw);
	velocity += inputMoveVector * (hacking ? 3.0f : 1.0f) * deltaTime;

	if (controller.jump)
	{
		if (grounded || hacking) // cheat code hold left trigger to moonjump
		{
			jumping = true;
			velocity.y = 0.075f;
		}
	}

	if (velocity.y > (GRAVITY * 50 * deltaTime)) // terminal velocity
		velocity.y += GRAVITY * deltaTime;

	// slow player down
	velocity.x /= FRICTION;
	velocity.z /= FRICTION;

	// round low velocity to 0
	if (abs(velocity.x) < 0.001)
		velocity.x = 0;
	if (abs(velocity.z) < 0.001)
		velocity.z = 0;
}

void PlayerController::collidePlayer(Player& player, const std::vector<Triangle>& collisionData)
{
	// reset important fields
	colResponseDelta = glm::vec3(0);
	grounded = false;

	resolveCollision(player, collisionData);
}

// return true when another iteration must be run to resolve all collision
void PlayerController::resolveCollision(Player& player, const std::vector<Triangle>& collisionData)
{
	colResponseDelta = glm::vec3(0.0f);

	std::vector<Triangle> groundCollisions;
	std::vector<Collision> wallCollisions;

	const float colSphereRadius = 0.35f;
	const float colSphereRadiusSquare = colSphereRadius * colSphereRadius;

	const glm::vec3 colliderOrigin = player.position + glm::vec3(0, colSphereRadius, 0);
	
	glm::vec3 ghostPosition = colliderOrigin + velocity;
	float playerGhostDistance = glm::length(velocity); // TODO prevent sqrt here
	
	const glm::vec3 direction = glm::normalize(velocity);

	// commented out this "continuous collision" so I can make sure that's not the problem
	const Ray playerRay{ colliderOrigin, direction };
	const Collision playerRayHit = Util::rayCast(playerRay, collisionData);
	
	const glm::vec3 colSphereOrigin = playerRayHit.dist < playerGhostDistance
		                                   ? playerRayHit.point
		                                   : ghostPosition; // there's a tri we would skip over, so instead tp player to here

	if(playerRayHit.dist < playerGhostDistance) // there's a try we would skip over
	{
		velocity = direction * (playerRayHit.dist - colSphereRadius);

		// recalculate these 
		ghostPosition = colliderOrigin + velocity;
		playerGhostDistance = glm::length(velocity);
		
	}
	
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
			// check if ground is actually under us or not
			Ray downRay = Ray{ ghostPosition + glm::vec3(0, 999, 0), glm::vec3(0, -1, 0) };
			Collision downCollision = Util::rayCast(downRay, curTri, true);

			if (downCollision.dist != INFINITY)
			{
				groundCollisions.emplace_back(curTri);

				grounded = true;
			}
		}
		else // handle wall collisions
    	{
			// we need to correct by this
			const float distancePastTri = (colSphereRadius - pointToPlaneDist);

			// angle at which we run into
			const float cosine = 1.0f - fabs(Util::cosBetweenV3(direction, curTri.n));
			
			const float hypotenuse = distancePastTri, adjacentSide = cosine * hypotenuse;
			const float oppositeSide = sqrt(pow(hypotenuse, 2) - pow(adjacentSide, 2));

			glm::vec3 curCollisionShift = curTri.n * (oppositeSide);

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

		colResponseDelta.y = 0;

		float highestDeltaY = -INFINITY;
    	// do this after ALL mods to velocity and colResponseDelta are done
		for (const Triangle& curTri : groundCollisions)
		{
			glm::vec3 newPlayerPos = player.position + velocity + colResponseDelta;

			const float y = ((curTri.n.x * (newPlayerPos.x - curTri.v0.x)
				+ curTri.n.z * (newPlayerPos.z - curTri.v0.z)) / -curTri.n.y)
				+ curTri.v0.y;

			const float deltaY = y - player.position.y;

			if (deltaY > highestDeltaY)
				highestDeltaY = deltaY;
		}

    	for(const Collision& collision : wallCollisions)
    	{
			if (collision.point.y <= highestDeltaY) // we're colliding with stuff under the ground we're on
			{
				//colResponseDelta -= collision.shiftBy;
			}
    	}

		if (highestDeltaY != -INFINITY)
		{
			colResponseDelta.y = highestDeltaY;
			velocity.y = 0;
		}
    }
}

// TODO implement this function
// ReSharper disable once CppMemberFunctionMayBeStatic
void PlayerController::animatePlayer(Player& playerIn)
{
	return;

	//AnimType newAnim;

	//if (isGrounded && length2V3(velocity) < 0.0000002) {
	//	newAnim = AnimType::idle;
	//}
	//else if (!isGrounded) {
	//	if (velocity.y > -0.01) {
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

void PlayerController::movePlayer(Player& player)
{
	// NAN check
	if (velocity.x != velocity.x)
		std::cout << "ERROR: velocity is " << Util::vecToStr(velocity) << std::endl;
	
	player.move(velocity + colResponseDelta);
	
	// NAN check	
	if (player.position.x != player.position.x)
		std::cout << "ERROR: position is " << Util::vecToStr(player.position) << std::endl;
}

bool PlayerController::isMoving() const
{
	return Util::length2V3(velocity) != 0.0f;
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
