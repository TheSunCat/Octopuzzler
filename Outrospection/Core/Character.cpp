#include "Character.h"

#include "Rendering/Resource.h"

#include "../Source.h"

Character::Character(const std::string& _charId, const glm::vec3& _pos) : charPosition(_pos)
{
	charId = _charId;

	Resource r("Characters/" + charId + "/idle/", "idle0.png");

	SimpleTexture billboardTex = getOutrospection()->textureManager.loadTexture(r);

	charBillboard = Billboard(billboardTex);
}
void Character::draw(Shader& _shader)
{
	charBillboard.draw(_shader, charPosition);
}