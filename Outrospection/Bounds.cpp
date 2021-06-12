#include "Bounds.h"

#include "Constants.h"
#include "Util.h"

Bounds::Bounds(BoundsShape _shape, const std::vector<float>& boundsArgs) : shape(_shape)
{
	pos = glm::vec2(SCR_WIDTH * boundsArgs[0], SCR_HEIGHT * boundsArgs[1]);
	
	switch(shape)
	{
	case BoundsShape::AABB:
		size = glm::vec2(SCR_WIDTH * boundsArgs[2], SCR_HEIGHT * boundsArgs[3]);
		break;
	case BoundsShape::Circle:
		radius2 = pow(SCR_WIDTH * boundsArgs[2], 2);
	}
}


bool Bounds::contains(const glm::vec2& point) const
{
	switch(shape)
	{
	case BoundsShape::AABB:
		return (point.x > pos.x && point.x < pos.x + size.x) && (point.y > pos.y && point.y < pos.y + size.y);
	case BoundsShape::Circle:
		return Util::dist2(pos, point) < radius2;
	}

	LOG_ERROR("Invalid bound shape?");
	return false;
}
