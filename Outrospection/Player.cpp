#include "Player.h"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

void Player::move(glm::vec3 deltaPos) {
	position.x += deltaPos.x;
	position.y += deltaPos.y;
	position.z += deltaPos.z;

	std::cout << "New pos: " << glm::to_string(position) << std::endl;
}

void Player::rotate(float yaw) {
	rotation.y = yaw;
}