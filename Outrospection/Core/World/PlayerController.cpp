#include "PlayerController.h"
#include "Core.h"

#include "Constants.h"
#include "Controller.h"
#include "Util.h"

#include <Outrospection.h>

#include "Core/World/PhysicsValues.h"
#include "Core/World/Player.h"

void PlayerController::acceleratePlayer(Player& player, const Controller& controller, glm::vec3 forward, glm::vec3 down, const float deltaTime)
{
    const bool hacking = (controller.leftTrigger >= 0.85f || controller.talk);
    // apparently, hacking is a side effect of debug mode - someone unimportant

    glm::vec3 inputMoveVector(0);

    const glm::vec3 right = glm::cross(forward, down);

    inputMoveVector += -forward * controller.lStickY;
    inputMoveVector += right * controller.lStickX;

    velocity += inputMoveVector * (hacking ? 5.0f : 1.0f);


    // friction
    velocity *= pow(0.99f, deltaTime * 60);

    if (controller.jump && controller.jump < 10) // can jump for 10 frames after hitting jump
    {
        if (groundTri || hacking) // cheat code hold left trigger to moonjump
        {
            jumping = true;

            LOG_DEBUG("jumping!");

            velocity = (-gravity * (player.jumpStrength));
        }
    }

    // apply the gravity of the situation
    velocity += (gravity * gravityStrength) * deltaTime;


    if(!groundTri)
    {
        //LOG_DEBUG("%f, %f", velocity.x, player.position.x - 20.0f);
    }

    frameDelta = velocity * deltaTime;
}

void PlayerController::collidePlayer(Player& player, const std::vector<Triangle>& collisionData, float deltaTime)
{
    const float playerCollisionRadius = 0.5f;

    glm::vec3 ghostPosition = player.position + glm::vec3(0, 0.5, 0) + frameDelta;

    groundTri = nullptr;

    auto intersectPoint = glm::vec3(0);
    float pointToPlaneDist = 0;
    for(const Triangle& tri : collisionData)
    {
        bool colliding = Util::intersectTriangleSphere(ghostPosition, playerCollisionRadius, tri, intersectPoint, pointToPlaneDist);


        if (colliding)
        {

            glm::vec3 normalComponent = glm::cross(tri.n, velocity);

            // we need to correct by this
            const float distancePastTri = (playerCollisionRadius - pointToPlaneDist);


            // angle at which we run into
            const float cosine = 1.0f - fabs(Util::cosBetweenV3(glm::normalize(velocity), tri.n));

            if (cosine < 0)
                continue;

            const float hypotenuse = distancePastTri, adjacentSide = cosine * hypotenuse;
            const float oppositeSide = sqrt(pow(hypotenuse, 2) - pow(adjacentSide, 2));


            glm::vec3 thing = glm::abs(glm::normalize((player.position + glm::vec3(0, 0.5, 0)) - intersectPoint));

            glm::vec3 inverseThing = 1.0f - thing;

            velocity += oppositeSide * tri.n;


            //velocity += tri.n * oppositeSide;

            //velocity *= glm::length(velocity) - pointToPlaneDist;
            
            frameDelta = velocity * deltaTime;

            groundTri = &tri;

            LOG_DEBUG("Colliding! %s", Util::vecToStr(thing));
        }
    }
}

// TODO implement this function
// ReSharper disable once CppMemberFunctionMayBeStatic
void PlayerController::animatePlayer(Player& player)
{
    return;
}

void PlayerController::movePlayer(Player& player, float deltaTime) const
{
    // NAN check
    if (std::isnan(frameDelta.x))
        LOG_ERROR("frameDelta is NaN!");

    player.move(frameDelta);
}

bool PlayerController::isMoving() const
{
    return Util::length2V3(velocity) != 0.0f;
}