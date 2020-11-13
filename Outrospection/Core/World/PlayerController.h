#pragma once

#include <glm/glm.hpp>

#include "Types.h"
#include "Core/World/CollisionMesh.h"

struct CollisionPackage;
class Controller;
class Player;

class PlayerController
{
public:
    void acceleratePlayer(Player& player, const Controller& controller, glm::vec3 forward, glm::vec3 down, float deltaTime);

    void collidePlayer(Player& player, const glm::vec3& gravity, const std::vector<CollisionMesh>& collision);

    bool isMoving() const;
private:
    CollisionPackage package{};

    glm::vec3 collideWithWorld(const glm::vec3& pos, const glm::vec3& vel, const std::vector<CollisionMesh>& collision);

    glm::vec3 velocity = glm::vec3(0.0f);

    const Triangle* groundTri = nullptr;
    bool jumping = false;

    float collisionRecursionDepth = 0;
};
