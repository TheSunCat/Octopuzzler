#include "Bounds.h"

#include "UIComponent.h"

#include "Constants.h"
#include "Util.h"

Bounds::Bounds(const UITransform& _transform, BoundsShape _shape) : shape(_shape), transform(_transform)
{ }

Bounds::Bounds() : shape(BoundsShape::None), transform(0,0,0,0) {}


bool Bounds::contains(const glm::vec2& point) const
{
    glm::vec2 pos = transform.getPos();
    glm::vec2 size = transform.getSize();

    switch(shape)
    {
    case BoundsShape::AABB:
        return (point.x > pos.x && point.x < pos.x + size.x) && (point.y > pos.y && point.y < pos.y + size.y);
    case BoundsShape::Circle:
        return Util::dist2(pos, point) < (size.x * size.x);
    }

    LOG_ERROR("Invalid bound shape?");
    return false;
}
