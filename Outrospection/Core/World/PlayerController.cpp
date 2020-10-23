#include "PlayerController.h"
#include "Core.h"

#include "Constants.h"
#include "Controller.h"
#include "Util.h"

#include "Core/World/Player.h"

void PlayerController::acceleratePlayer(const Controller& controller, const float deltaTime, const float yaw)
{
    const bool hacking = (controller.leftTrigger >= 0.85f || controller.talk);
    // apparently, hacking is a side effect of debug mode - smug

    const glm::vec3 inputMoveVector = processInput(controller, yaw);
    velocity += inputMoveVector * (hacking ? 3.0f : 1.0f) * deltaTime;

    if (controller.jump && controller.jump < 10) // can jump for 10 frames after hitting jump
    {
        if (grounded || hacking) // cheat code hold left trigger to moonjump
        {
            LOG_DEBUG("jumping");

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
    if (abs(velocity.x) < 0.001f)
        velocity.x = 0;
    if (abs(velocity.z) < 0.001f)
        velocity.z = 0;
}

void PlayerController::collidePlayer(Player& player, const std::vector<Triangle>& collisionData)
{
    // reset important fields
    colResponseDelta = glm::vec3(0);
    grounded = false;

    resolveCollision(player, collisionData);
}

void PlayerController::resolveCollision(Player& player, const std::vector<Triangle>& collisionData)
{
    colResponseDelta = glm::vec3(0.0f);

    std::vector<Triangle> groundCollisions;
    std::vector<Collision> wallCollisions;

    const float colSphereRadius = 0.35f;
    const float colSphereRadiusSquare = colSphereRadius * colSphereRadius;

    const glm::vec3 colliderOrigin = player.position + glm::vec3(0, colSphereRadius, 0);

    glm::vec3 ghostPosition = colliderOrigin + velocity;
    float playerGhostDistanceSq = Util::length2V3(velocity);

    const glm::vec3 direction = glm::normalize(velocity);

    // TODO make this a spherecast?
    const Ray playerRay{colliderOrigin, direction};
    const Collision playerRayHit = Util::rayCast(playerRay, collisionData);

    const float rayHitDistSq = playerRayHit.dist * playerRayHit.dist;

    const glm::vec3 colSphereOrigin = rayHitDistSq < playerGhostDistanceSq
                                           ? glm::normalize(playerRay.direction) * playerRayHit.dist
                                           : ghostPosition; // there's a tri we would skip over, so instead tp player to here

    if (rayHitDistSq < playerGhostDistanceSq) // there's a tri we would skip over
    {
        velocity = direction * (playerRayHit.dist - colSphereRadius);

        // recalculate these 
        ghostPosition = colliderOrigin + velocity;
    }

    int numCollisions = 0;

    auto intersectPoint = glm::vec3(0);
    for (const Triangle& curTri : collisionData)
    {
        float pointToPlaneDist = 0;

        //bool outsidePlane = false;
        if (Util::intersectTriangleSphere(colliderOrigin, colSphereRadius, curTri, intersectPoint, pointToPlaneDist))
        {
            // --------------------------------------------------
            // we found a collision!
            // push the character outside of the triangle we intersected

            if (fabs(curTri.n.y) > 0.1) // we are colliding with ground
            {
                // check if ground is actually under us or not
                Ray downRay = Ray{ ghostPosition + glm::vec3(0, 999, 0), glm::vec3(0, -1, 0) };
                Collision downCollision = Util::rayCast(downRay, curTri, true);

                // check if we're on a ledge, therefore need special treatment due to sphere collider not being flat on bottom
                ledgeHitPoint = (downCollision.dist == INFINITY && (intersectPoint.y - player.position.y) <
                    colSphereRadiusSquare / 1.5f)
                    ? intersectPoint
                    : glm::vec3(0.0f);

                if ((downCollision.dist != INFINITY) || Util::length2V3(ledgeHitPoint))
                {
                    groundCollisions.emplace_back(curTri);

                    if (jumping)
                        jumping = false;

                    grounded = true;
                }
            }
            else // handle wall collisions
            {
                float playerFollowsNorm = glm::dot(velocity, curTri.n);
                if (playerFollowsNorm > 0)
                    continue;

                float intersectYDiff = intersectPoint.y - player.position.y;

                // TODO fix where first time it does snap, maybe ledgeHitPoint isn't set?
                if (Util::length2V3(ledgeHitPoint) && fabs(intersectYDiff) <= 0.075f)
                {
                    //LOG_DEBUG("%f skipping wall", intersectYDiff);

                    //continue;
                }

                LOG_DEBUG("%f", intersectYDiff);

                // we need to correct by this
                const float distancePastTri = (colSphereRadius - pointToPlaneDist);

                glm::vec3 dirNoY = glm::vec3(direction.x, 0, direction.z);
                if (Util::length2V3(dirNoY) == 0.0f)
                    dirNoY = -curTri.n;

                // angle at which we run into
                const float cosine = 1.0f - fabs(Util::cosBetweenV3(dirNoY, curTri.n));

                const float hypotenuse = distancePastTri, adjacentSide = cosine * hypotenuse;
                const float oppositeSide = sqrt(pow(hypotenuse, 2) - pow(adjacentSide, 2));

                glm::vec3 curCollisionShift = curTri.n * (oppositeSide);

                colResponseDelta += curCollisionShift;

                Collision col{ pointToPlaneDist, intersectPoint, curCollisionShift };
                wallCollisions.emplace_back(col);
            }

            numCollisions++;
        }
    }

    if (numCollisions != 0 || !groundCollisions.empty())
    {
        if (numCollisions > 0)
            colResponseDelta /= float(numCollisions); // average our collision responses

        colResponseDelta.y = 0;

        float highestGroundDeltaY = -INFINITY;
        // do this after ALL mods to velocity and colResponseDelta are done
        for (const Triangle& curTri : groundCollisions)
        {
            glm::vec3 newPlayerPos = player.position + velocity + colResponseDelta;

            const float y = ((curTri.n.x * (newPlayerPos.x - curTri.verts[0].x)
                    + curTri.n.z * (newPlayerPos.z - curTri.verts[0].z)) / -curTri.n.y)
                + curTri.verts[0].y;

            const float deltaY = y - player.position.y;

            if (deltaY > highestGroundDeltaY)
                highestGroundDeltaY = deltaY;
        }

        if (highestGroundDeltaY != -INFINITY)
        {
            colResponseDelta.y = highestGroundDeltaY;
            velocity.y = 0; // Util::clamp(velocity.y, 0.0f, fabs(velocity.y));
        }
    }

    // edge case
    // haha
    if (Util::length2V3(ledgeHitPoint) && !jumping) // player is on a ledge
    {
        // TODO we need to move this raycast to the closest point on the ledge tri to the player so we stop pecking vibrating aaaaaaa (when moving along an edge)
        Ray ledgeRay = Ray{ledgeHitPoint, glm::vec3(0, 1, 0)};
        ledgeRay.origin.y -= colSphereRadius / 2.0f;

        glm::vec3 intersectPoint = glm::vec3();
        bool hit = Util::intersectRaySegmentSphere(ledgeRay, colSphereOrigin, colSphereRadiusSquare / 1.5f,
                                                   intersectPoint);

        if (!hit)
            ledgeHitPoint = glm::vec3(0.0f);
        else
        {
            velocity.y = Util::clamp(velocity.y, 0.0f, fabs(velocity.y));
            grounded = true;

            //std::cout << "RESET velocity.y\n";
        }
    }

    //std::cout << grounded << ", " << velocity.y << '\n';
}

// TODO implement this function
// ReSharper disable once CppMemberFunctionMayBeStatic
void PlayerController::animatePlayer(Player& player)
{
    return;

    //AnimType newAnim;

    //if (isGrounded && length2V3(velocity) < 0.0000002) {
    //    newAnim = AnimType::idle;
    //}
    //else if (!isGrounded) {
    //    if (velocity.y > -0.01) {
    //        newAnim = AnimType::jump;
    //    }
    //    else {
    //        newAnim = AnimType::fall;
    //    }
    //}
    //else {
    //    newAnim = AnimType::walk;
    //}

    //if (newAnim != pastAnim)
    //    playerIn->setAnimation(newAnim);

    //pastAnim = newAnim;
}

void PlayerController::movePlayer(Player& player) const
{
    // NAN check
    if (std::isnan(velocity.x))
    {
        LOG_ERROR("velocity is NaN!");
    }

    if (std::isnan(colResponseDelta.x) || std::isnan(colResponseDelta.y) || std::isnan(colResponseDelta.z))
        LOG_ERROR("colResponseDelta is NaN!");

    player.move(velocity + colResponseDelta);
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

    if (Util::length2V3(inputVector) > 1.0)
    {
        // normalize if we would move too fast
        inputVector = glm::normalize(inputVector);
    }

    return inputVector;
}
