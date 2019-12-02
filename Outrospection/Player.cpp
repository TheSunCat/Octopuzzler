#include "Player.h"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

Player::Player(const glm::vec3& _startPos, const glm::vec3& _startRot) : playerPosition(_startPos),
playerRotation(_startRot), playerCharacter("player")
{

}

void Player::move(const glm::vec3& deltaPos)
{
	playerPosition.x += deltaPos.x;
	playerPosition.y += deltaPos.y;
	playerPosition.z += deltaPos.z;

	std::cout << "New pos: " << glm::to_string(playerPosition) << std::endl;
}

void Player::rotate(float yaw)
{
	playerRotation.y = yaw;
}

void Player::draw(Shader& _shader)
{
	glm::mat4 modelMat = glm::mat4(1.0f);

	// rotate each axis individually bc ogl dum
	modelMat = glm::rotate(modelMat, glm::radians(playerRotation.x), glm::vec3(1, 0, 0));
	modelMat = glm::rotate(modelMat, glm::radians(playerRotation.y), glm::vec3(0, 1, 0));
	modelMat = glm::rotate(modelMat, glm::radians(playerRotation.z), glm::vec3(0, 0, 1));

	// Translate model
	modelMat = glm::translate(modelMat, playerPosition);

	_shader.setMat4("model", modelMat);
	
	playerCharacter.draw(_shader);
}
