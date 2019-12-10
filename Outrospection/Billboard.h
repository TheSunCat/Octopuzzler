#pragma once
#include "Shader.h"

class Billboard {
public:
	Billboard();

	void draw(const Shader& _shader, unsigned int _texture, const glm::vec3& _pos);

private:
	unsigned int quadVAO;
};