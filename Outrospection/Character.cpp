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
		case fall: {
			aType = "fall";
			break;
		}
		default: {
			std::cerr << "invalid anim type :(" << std::endl;
		}
		}

		for (int i = 0; i < a.frameCount; i++)
		{
			unsigned int tex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "/");
			a.frames.push_back(tex);
		}

		std::pair<AnimType, Animation> pair(a.animType, a);

		animations.insert(pair);
	}

	setAnimation(AnimType::idle);
}

void Character::setAnimation(AnimType _animType)
{
	curAnim = animations.at(_animType);
}

void Character::draw(Shader& _shader)
{
	charBillboard.draw(_shader, charPosition, curAnim);
	
	curAnim.frameTally++;

	if (curAnim.frameTally > curAnim.frameLength) {
		curAnim.frameTally = 0;
		if (curAnim.curFrame < (curAnim.frameCount - 1))
			curAnim.curFrame++;
		else
			curAnim.curFrame = 0;
	}
}