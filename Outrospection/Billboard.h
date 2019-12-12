#pragma once
#include "Shader.h"
#include "Types.h"

class Billboard {
public:
	Billboard();

	void draw(Shader& _shader, const glm::vec3& _pos, unsigned int _texture);

	void draw(Shader& _shader, const glm::vec3& _pos, const Animation& a);

private:
	unsigned int quadVAO = 0;
};