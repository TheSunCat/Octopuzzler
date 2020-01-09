#include "Player.h"
#include <iostream>

Player::Player(const glm::vec3& _startPos, const glm::vec3& _startRot) : playerPosition(_startPos),
playerRotation(_startRot), playerCharacter("deborah", _startPos, { Animation {AnimType::walk, 8, 6}, Animation {AnimType::jump, 5, 12} , Animation {AnimType::fall, 6, 12} })
{

}

void Player::move(const glm::vec3& deltaPos)
{
	playerPosition += deltaPos * playerSpeed;

	playerCharacter.charPosition = playerPosition;
}

void Player::rotate(float yaw)
{
	playerRotation.y = yaw;
}

void Player::setAnimation(AnimType _animType)
{
	playerCharacter.setAnimation(_animType);
}

void Player::draw(Shader& _shader)
{
	playerCharacter.draw(_shader);
}
