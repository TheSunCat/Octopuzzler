#pragma once
#include <string>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Billboard.h"
#include <unordered_map>
#include "Types.h"
#include "Util.h"

class Character {
public:
	Character() = default;

	Character(const std::string& _charId, const glm::vec3& _pos, std::vector<Animation> anims);

	void setAnimation(int _index);

	void draw(Shader& _shader);

	glm::vec3 charPosition;

private:
	std::string charId;

	int curAnimIndex = 0;
	std::vector<Animation> animations;

	Billboard charBillboard;
};