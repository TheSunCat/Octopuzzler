#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Core/Rendering/Billboard.h"

class Shader;

class Character {
public:
	Character() = default;

	Character(const std::string& _charId, const glm::vec3& _pos);

	void draw(Shader& _shader) const;

	glm::vec3 charPosition;

private:
	std::string charId;

	Billboard charBillboard;
};