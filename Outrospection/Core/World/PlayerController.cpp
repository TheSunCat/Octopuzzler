#include "PlayerController.h"
#include "Core.h"

#include "Constants.h"
#include "Controller.h"
#include "Util.h"

#include <Outrospection.h>

#include "Core/World/CollisionMesh.h"
#include "Core/World/PhysicsValues.h"
#include "Core/World/Player.h"

void PlayerController::acceleratePlayer(Player& player, const Controller& controller, glm::vec3 forward, glm::vec3 down, const float deltaTime)
{
    velocity /= deltaTime;

    const bool hacking = (controller.leftTrigger >= 0.85f || controller.talk);
    // apparently, hacking is a side effect of debug mode - someone unimportant

    glm::vec3 inputMoveVector(0);

    const glm::vec3 right = glm::cross(forward, down);

    inputMoveVector += -forward * controller.lStickY;
    inputMoveVector += right * controller.lStickX;

    velocity += inputMoveVector * (hacking ? 5.0f : 1.0f);

    //velocity *= pow(0.99f, deltaTime * 60); // friXion

    if (controller.jump && controller.jump < 10) // can jump for 10 frames after hitting jump
    {
        if (groundTri || hacking) // cheat code hold left trigger to moonjump
        {
            jumping = true;

            //LOG_DEBUG("jumping!");

            velocity = (-Physics::gravity * (player.jumpStrength));
        }
    }

    velocity += Physics::gravity * Physics::gravityStrength * deltaTime;

    //LOG("Velocity = %sm/s", Util::vecToStr(velocity));

    velocity *= deltaTime;
}

void PlayerController::collidePlayer(Player& player, const glm::vec3& gravity, const std::vector<CollisionMesh>& collision)
{
    constexpr float sphereRadius = 50;
    constexpr float sphereRadiusSq = sphereRadius * sphereRadius;

    glm::vec3 finalPosition = player.position + velocity;
    if(Util::dist2(finalPosition, glm::vec3(0)) < sphereRadiusSq)
    {
        velocity = glm::vec3(0);// gravity;
        finalPosition = player.position + velocity;
    }



    // TODO move the player
    player.moveTo(finalPosition);
}

glm::vec3 PlayerController::collideWithWorld(const glm::vec3& pos, const glm::vec3& vel, const std::vector<CollisionMesh>& collision)
{
    float veryCloseDistance = 0.005f;

    // do we need to be :worried:
    if (collisionRecursionDepth > 5)
        return pos;

    // :worried:
    package.velocity = vel;
    package.normalizedVelocity = glm::normalize(vel);
    package.basePoint = pos;
    package.foundCollision = false;
    package.nearestDistance = INFINITY;

    // check collision!
    for(const CollisionMesh& col : collision)
    {
        col.collide(package);
    }


    if(!package.foundCollision)
    {
        return pos + vel;
    }


    // if we did collide

    // "ghost" position
    glm::vec3 destinationPoint = pos + vel;
    glm::vec3 newBasePoint = pos;


    if(package.nearestDistance >= veryCloseDistance)
    {
        glm::vec3 v = vel;
        Util::setLength(v, package.nearestDistance - veryCloseDistance);

        newBasePoint = package.basePoint + v;

        v = glm::normalize(v);
        package.intersectionPoint -= veryCloseDistance * v;
    }

    // determine the sliding plane
    glm::vec3 slidePlaneOrigin = package.intersectionPoint;
    glm::vec3 slidePlaneNormal = glm::normalize(newBasePoint - package.intersectionPoint);

    Plane slidingPlane(slidePlaneOrigin, slidePlaneNormal);

    glm::vec3 newDestinationPoint = destinationPoint - slidingPlane.signedDistanceTo(destinationPoint) * slidePlaneNormal;

    // generate the slide vector, which will become velocity in next iteration
    glm::vec3 newVelocityVector = newDestinationPoint - package.intersectionPoint;

    // recurse

    // don't recurse if velocity is very small
    if(Util::length2(newVelocityVector) < (veryCloseDistance * veryCloseDistance))
    {
        return newBasePoint;
    }

    collisionRecursionDepth++;
    return collideWithWorld(newBasePoint, newVelocityVector, collision);
}

bool PlayerController::isMoving() const
{
    return Util::length2(velocity) != 0.0f;
}