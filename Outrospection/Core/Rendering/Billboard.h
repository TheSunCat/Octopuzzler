#pragma once

#include "Shader.h"
#include "SimpleTexture.h"

class Billboard {
public:
	Billboard() = default;

	Billboard(const SimpleTexture& _tex);

	void draw(const Shader& _shader, const glm::vec3& _pos) const;

	void draw(const Shader& _shader, const glm::vec3& _pos, const SimpleTexture& _tex) const;

private:
	unsigned int quadVAO = 0;
	SimpleTexture texture;
};