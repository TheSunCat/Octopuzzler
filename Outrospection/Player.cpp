#include "Player.h"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

void Player::move(const glm::vec3& deltaPos) {
	position.x += deltaPos.x;
	position.y += deltaPos.y;
	position.z += deltaPos.z;

	std::cout << "New pos: " << glm::to_string(position) << std::endl;
}

void Player::rotate(float yaw)
{
	rotation.y = yaw;
}

void Player::draw(const Shader& shader)
{
	glm::mat4 modelMat = glm::mat4(1.0f);

	// rotate each axis individually bc ogl dum
	modelMat = glm::rotate(modelMat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	modelMat = glm::rotate(modelMat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	modelMat = glm::rotate(modelMat, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	// Translate model
	modelMat = glm::translate(modelMat, position);

	shader.setMat4("model", modelMat);
	//model.Draw(shader);
}
