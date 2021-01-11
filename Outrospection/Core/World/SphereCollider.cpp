#include "Core/World/SphereCollider.h"

#include "Types.h"
#include "Util.h"

SphereCollider::SphereCollider(const glm::vec3& center, float radius) : mCenter(center), mRadius(radius)
{

}

Transform SphereCollider::trans() const {
    return Transform{ mCenter, glm::vec3(mRadius) };
}

glm::vec3 SphereCollider::findFurthestPoint(const Transform* transform, glm::vec3 direction) const
{
    return mCenter + transform->pos().get()
        + mRadius * glm::normalize(direction) * Util::major(transform->scl());
}

ManifoldPoints SphereCollider::testCollision(const Transform* transform, const Collider* collider,
    const Transform* colliderTransform) const
{
    return collider->testCollision(colliderTransform, this, transform);
}

ManifoldPoints SphereCollider::testCollision(const Transform* transform, const SphereCollider* collider,
    const Transform* colliderTransform) const
{
    return Util::intersectSphereSphere(this, transform, collider, colliderTransform);
}