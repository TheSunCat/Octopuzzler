#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>

class Player {
public:
	Player(glm::vec3 _startPos, glm::vec3 _startRot) : position(_startPos), rotation(_startRot)
	{

	}

	void move(glm::vec3 deltaPos);

	void rotate(float yaw);

	glm::vec3 position;
	glm::vec3 rotation;
};

#endif