#include "Character.h"

Character::Character(const std::string& _charId, const glm::vec3& _pos, std::vector<Animation> anims) : charPosition(_pos)
{
	charId = _charId;

	for (Animation a : anims) {

		std::string aType;

		switch (a.animType) {
		case idle: {
			aType = "idle";
			break;
		}
		case walk: {
			aType = "walk";
			break;
		}
		case jump: {
			aType = "jump";
			break;
		}
		default: {
			std::cerr << "invalid anim type :(" << std::endl;
		}
		}

		unsigned int firstTexture = TextureFromFile((aType.append("0")).c_str(), "./res/ObjectData/Characters/" + charId + "/");
		a.texId = firstTexture;

		for (int i = 1; i < a.frameCount; i++)
		{
			subTextureFromFile((aType.append(std::to_string(i))).c_str(), "./res/ObjectData/Characters/" + charId + "/", firstTexture);
		}

		std::pair<AnimType, Animation> insert(a.animType, a);

		animations.insert(insert);
	}

	
}

void Character::draw(Shader& _shader)
{
	unsigned int curTexture = animations.at(AnimType::walk).texId;

	charBillboard.draw(_shader, curTexture, charPosition);
}