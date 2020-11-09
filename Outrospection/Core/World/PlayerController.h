#pragma once

#include <glm/glm.hpp>

#include "Types.h"

class Controller;
class Player;

class PlayerController
{
public:
    void acceleratePlayer(Player& player, const Controller& controller, glm::vec3 forward, glm::vec3 down, float deltaTime);

    void collidePlayer(Player& player, const std::vector<Triangle>& collisionData, float deltaTime);

    void animatePlayer(Player& player);

    void movePlayer(Player& player, float deltaTime) const;

    bool isMoving() const;
private:
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 frameDelta = glm::vec3(0.0f);

    const Triangle* groundTri = nullptr;
    bool jumping = false;
};
