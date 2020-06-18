#include "Character.h"

#include "Source.h"

#include "Core/Rendering/Resource.h"

Character::Character(const std::string& _id, const glm::vec3& _pos) : position(_pos)
{
	id = _id;

	Resource r("Characters/" + id + "/idle/", "idle0.png");

	const SimpleTexture texture = getOutrospection()->textureManager.loadTexture(r);

	billboard = Billboard(texture);
}
void Character::draw(Shader& _shader) const
{
	billboard.draw(_shader, position);
}