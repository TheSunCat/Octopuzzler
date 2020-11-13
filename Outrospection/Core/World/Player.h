#pragma once

#include <glm/glm.hpp>

#include "Core/World/Character.h"

class Shader;

class Player
{
public:
    Player() = default;

    explicit Player(const glm::vec3& _startPos);

    void moveBy(const glm::vec3& deltaPos);

    void moveTo(const glm::vec3& newPos);

    //void setAnimation(AnimType _animType);

    void draw(Shader& _shader) const;

    glm::vec3 position{};
    Character character;

    //Collider& collider;

    float eyeHeight = 0.7f;
    float moveSpeed = 1.0f;

    float jumpStrength = 9.8f;
};
