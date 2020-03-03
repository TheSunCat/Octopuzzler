#include "Character.h"
#include <unordered_map>

Character::Character(const std::string& _charId, const glm::vec3& _pos, std::vector<Animation> anims) : charPosition(_pos)
{
	charId = _charId;

	for (Animation a : anims) {

		std::string aType = animTypeMap.at(a.animType);

		for (int i = 0; i < a.frameCount; i++)
		{
			// Bottom
			unsigned int bottomTex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "");
			a.frames.push_back(bottomTex);

			//// Legs
			//unsigned int legTex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "/legs");
			//a.frames.push_back(legTex);

			//// Arms
			//unsigned int armTex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "/arms");
			//a.frames.push_back(armTex);

			//// Torso
			//unsigned int torsoTex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "/torso");
			//a.frames.push_back(torsoTex);

			//// Head
			//unsigned int headTex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "/head");
			//a.frames.push_back(headTex);

			//// Face
			//unsigned int faceTex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "/face");
			//a.frames.push_back(faceTex);

			//// Top
			//unsigned int topTex = TextureFromFile((aType + std::to_string(i) + ".png").c_str(), "./res/ObjectData/Characters/" + charId + "/top");
			//a.frames.push_back(topTex);
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