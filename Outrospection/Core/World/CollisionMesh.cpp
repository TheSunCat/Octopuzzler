#include "CollisionMesh.h"

#include "Util.h"

CollisionMesh::CollisionMesh(std::string& _name, std::vector<Triangle>& colData) :
    name(std::move(_name)),
    collisionData(std::move(colData))
{
    
}

void CollisionMesh::collide(CollisionPackage& package) const
{
    for(const Triangle& tri : collisionData)
    {
        collidesWith(package, tri);
    }
}

void CollisionMesh::collidesWith(CollisionPackage& package, const Triangle& tri) const
{
    // thanks to https://peroxide.dk/papers/collision/collision.pdf for code inspiration

    auto plane = Plane(tri);


    if (!plane.isFrontFacingTo(package.normalizedVelocity))
    {
        //return; // we are going in the direction of the normal
        //tri.n = -tri.n;
        plane = Plane(tri);;

    }


    float t0, t1;
    bool embeddedInPlane = false;

    float signedDistToPlane = plane.signedDistanceTo(package.basePoint);


    float normalDotVelocity = glm::dot(plane.n, package.velocity);

    if (normalDotVelocity == 0.0f) // we are perpendicular to the plane
    {
        if (abs(signedDistToPlane) >= 1.0f) // not embedded in plane
        {
            return;
        }
        else
        {
            LOG_DEBUG("checking collision with %s", Util::vecToStr(tri.n));
            // embedded in plane
            // we intersect it the whole time
            embeddedInPlane = true;
            t0 = 0;
            t1 = 1;
        }
    }
    else
    {
        t0 = (1.0f - signedDistToPlane) / normalDotVelocity;
        t1 = (-1.0f - signedDistToPlane) / normalDotVelocity;

        if (t0 > t1) // swap them 
        {
            Util::swap(t0, t1);
        }

        if (t0 > 1.0f || t1 < 0.0f)
        {
            // both values are out of range [0, 1]
            return;
        }

        t0 = Util::clamp(t0, 0.0f, 1.0f);
        t1 = Util::clamp(t1, 0.0f, 1.0f);
    }


    // OK, at this point we have two time values t0 and t1
    // between which our swept sphere intersects with the plane.
    // If any collision is to occur, it will happen between t0 and t1.


    glm::vec3 collisionPoint;
    bool foundCollision = false;
    float t = 1.0f;

    // Check for collision inside triangle.
    // must be at time = t0
    if (!embeddedInPlane)
    {
        glm::vec3 planeIntersectPoint = (package.basePoint - plane.n) + (t0 * package.velocity);

        if (Util::pointInTriangle(planeIntersectPoint, tri))
        {
            //LOG_DEBUG("point in tri");

            foundCollision = true;
            t = t0;
            collisionPoint = planeIntersectPoint;
        } else
        {
            //LOG_DEBUG("point in tri NOT");
        }
    }



    // swept sphere against points and edges
    // scary
    if (!foundCollision)
    {
        float velocityLength2 = Util::length2(package.velocity);

        float a, b, c; // quadratic equation
        float newT;

        // solve a*t² + b*t + c = 0
        a = velocityLength2;

        // check verts
        for (auto vert : tri.verts)
        {
            b = 2.0f * glm::dot(package.velocity, package.basePoint - vert);
            c = Util::length2(vert - package.basePoint) - 1.0f;
            if (Util::lowestRoot(a, b, c, t, newT))
            {
                t = newT;
                foundCollision = true;
                collisionPoint = vert;
            }
        }


        // check against edges
        for (unsigned int i = 0; i < 3; i++)
        {
            glm::vec3 edge = tri.verts[(i + 1) % 3] - tri.verts[i];
            glm::vec3 baseToVert = tri.verts[i] - package.basePoint;

            float edgeLength2 = Util::length2(edge);
            float edgeDotVelocity = glm::dot(edge, package.velocity);
            float edgeDotBaseToVert = glm::dot(edge, baseToVert);

            a = edgeLength2 * velocityLength2 + (edgeDotVelocity * edgeDotVelocity);
            b = edgeLength2 * (2.0f * glm::dot(package.velocity, baseToVert)) - 2.0f * edgeDotVelocity * edgeDotBaseToVert;
            c = edgeLength2 * (1.0f - Util::length2(baseToVert)) + edgeDotBaseToVert * edgeDotBaseToVert;

            // does the swept sphere collide against infinite edge?
            if (Util::lowestRoot(a, b, c, t, newT))
            {
                // check if intersection is actually within segment
                float f = (edgeDotVelocity * newT - edgeDotBaseToVert) / edgeLength2;
                if (f >= 0 && f <= 1)
                { // intersection is within segment
                    t = newT;
                    foundCollision = true;
                    collisionPoint = tri.verts[i] + f * edge;
                }
            }
        }
    }

    // finally done!
    if (foundCollision)
    {
        // t = time of collision
        float distToCollision = t * glm::length(package.velocity);

        // is this the closest hit or nah
        if (distToCollision < package.nearestDistance)
        {
            package.nearestDistance = distToCollision;
            package.intersectionPoint = collisionPoint;
            package.foundCollision = foundCollision;

        }
    }
}
