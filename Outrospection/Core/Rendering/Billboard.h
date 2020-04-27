#pragma once

#include "Shader.h"
#include "SimpleTexture.h"

class Billboard {
public:
	Billboard() = default;

	Billboard(const SimpleTexture& _tex);

	void draw(Shader& _shader, const glm::vec3& _pos);

	void draw(Shader& _shader, const glm::vec3& _pos, SimpleTexture& _tex);

private:
	unsigned int quadVAO = 0;
	SimpleTexture texture;
};