#include "Collider.h"

#include "Types.h"

Collider::Collider()
{
}

Transform Collider::trans() const
{
    return {};
}

glm::vec3 Collider::findFurthestPoint(const Transform* transform, glm::vec3 direction) const
{
    return glm::vec3();
}
