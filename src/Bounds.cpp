#include "Bounds.h"

#include <Core/UI/UIComponent.h>

#include "Constants.h"
#include "Util.h"

Bounds::Bounds(BoundsShape _shape, const std::vector<float>& boundsArgs) : shape(_shape)
{
    // TODO scale with screen size? how?

    //glm::vec2 res = Util::curResolution();

    pos = glm::vec2(boundsArgs[0], boundsArgs[1]);
	
    switch(shape)
    {
    case BoundsShape::AABB:
        size = glm::vec2(boundsArgs[2], boundsArgs[3]);
        break;
    case BoundsShape::Circle:
        radius2 = pow(boundsArgs[2], 2);
    }
}

Bounds::Bounds(const UITransform& transform)
{
    shape = BoundsShape::AABB;

    pos = transform.getPos();
    size = transform.getSize();
}

Bounds::Bounds() : shape(BoundsShape::None) {}


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
