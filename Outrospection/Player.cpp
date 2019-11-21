#include "Player.h"

void Player::move(glm::vec3 deltaPos) {
	position += deltaPos;
}

void Player::rotate(float yaw) {
	rotation.y = yaw;
}