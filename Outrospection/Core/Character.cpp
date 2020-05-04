#include "Character.h"

#include "Source.h"

#include "Core/Rendering/Resource.h"

Character::Character(const std::string& _charId, const glm::vec3& _pos) : charPosition(_pos)
{
	charId = _charId;

	Resource r("Characters/" + charId + "/idle/", "idle0.png");

	SimpleTexture billboardTex = getOutrospection()->textureManager.loadTexture(r);

	charBillboard = Billboard(billboardTex);
}
void Character::draw(Shader& _shader) const
{
	charBillboard.draw(_shader, charPosition);
}