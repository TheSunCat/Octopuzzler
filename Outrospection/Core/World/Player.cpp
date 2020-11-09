#include "Player.h"

Player::Player(const glm::vec3& _startPos) : position(_startPos), character("deborah", _startPos)
{
}

void Player::move(const glm::vec3& deltaPos)
{
    position += deltaPos;// *moveSpeed; // TODO not have deltaTime here

    character.position = position;
}

//void Player::setAnimation(AnimType _animType)
//{
//    character.setAnimation(_animType);
//}

void Player::draw(Shader& _shader) const
{
    character.draw(_shader);
}
