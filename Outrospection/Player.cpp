#include "Player.h"
#include <iostream>

Player::Player(const glm::vec3& _startPos, const glm::vec3& _startRot) : playerPosition(_startPos),
playerRotation(_startRot), playerCharacter("deborah", _startPos, { Animation {AnimType::walk, 8, 6}, Animation {AnimType::jump, 11, 6} })//, Animation {AnimType::idle, 0, 1} })
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

void Player::setAnimation(int _index)
{
	playerCharacter.setAnimation(_index);
}

void Player::draw(Shader& _shader)
{
	playerCharacter.draw(_shader);
}
