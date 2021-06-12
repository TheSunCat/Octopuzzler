#include "PlayerController.h"

#include "Controller.h"
#include "Util.h"

#include <Outrospection.h>

#include "PhysicsValues.h"
#include "Player.h"
#include "SphereCollider.h"

PlayerController::PlayerController(const Player& player)
    : playerBody(player.position, glm::vec3(0), glm::vec3(1.0f), new SphereCollider())
{
    
}

void PlayerController::acceleratePlayer(Player& player, const Controller& controller, glm::vec3 forward, glm::vec3 down, const float deltaTime)
{
    glm::vec3 inputMoveVector(0);

    const glm::vec3 right = glm::cross(forward, down);

    inputMoveVector += -forward * controller.lStickY;
    inputMoveVector += right * controller.lStickX;
    
    playerBody.applyForce(inputMoveVector);


    if (controller.jump && (controller.jump < 10 || controller.talk)) // can jump for 10 frames after hitting jump
    {
        playerBody.applyForce(-Physics::gravity * (player.jumpStrength));
        LOG("JUMP");
    }

    //LOG("%f, %f, %f", playerBody.force.x, playerBody.force.y, playerBody.force.z);
}

bool PlayerController::isMoving() const
{
    return playerBody.isMoving();
}