#pragma once

#include <glm/glm.hpp>

#include "Types.h"
#include "Core/World/Rigidbody.h"

struct CollisionPackage;
class Controller;
class Player;

class PlayerController
{
public:
    PlayerController(const Player& player);

    void acceleratePlayer(Player& player, const Controller& controller, glm::vec3 forward, glm::vec3 down, float deltaTime);

    bool isMoving() const;
    Rigidbody playerBody;
private:
};
