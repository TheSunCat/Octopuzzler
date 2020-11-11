#include "PlayerController.h"
#include "Core.h"

#include "Constants.h"
#include "Controller.h"
#include "Util.h"

#include <Outrospection.h>

#include "Core/World/PhysicsValues.h"
#include "Core/World/Player.h"

void PlayerController::acceleratePlayer(Player& player, const Controller& controller, glm::vec3 forward, glm::vec3 down, const float deltaTime)
{
    const bool hacking = (controller.leftTrigger >= 0.85f || controller.talk);
    // apparently, hacking is a side effect of debug mode - someone unimportant

    glm::vec3 inputMoveVector(0);

    const glm::vec3 right = glm::cross(forward, down);

    inputMoveVector += -forward * controller.lStickY;
    inputMoveVector += right * controller.lStickX;

    velocity += inputMoveVector * (hacking ? 5.0f : 1.0f);


    // friction
    velocity *= pow(0.99f, deltaTime * 60);

    if (controller.jump && controller.jump < 10) // can jump for 10 frames after hitting jump
    {
        if (groundTri || hacking) // cheat code hold left trigger to moonjump
        {
            jumping = true;

            LOG_DEBUG("jumping!");

            velocity = (-gravity * (player.jumpStrength));
        }
    }

    // apply the gravity of the situation
    velocity += (gravity * gravityStrength) * deltaTime;


    if(!groundTri)
    {
        //LOG_DEBUG("%f, %f", velocity.x, player.position.x - 20.0f);
    }

    frameDelta = velocity * deltaTime;
}

void PlayerController::collidePlayer(Player& player, const std::vector<Triangle>& collisionData, float deltaTime)
{
    const float playerCollisionRadius = 0.5f;

    glm::vec3 basePoint = player.position + glm::vec3(0, 0.5, 0);

    groundTri = nullptr;

    glm::vec3 normalizedVelocity = glm::normalize(velocity);

    float closestCollision = INFINITY;
    glm::vec3 intersectionPoint;
    bool collided = false;

    for(const Triangle& tri : collisionData)
    {
        // thanks to https://peroxide.dk/papers/collision/collision.pdf for code inspiration

        auto plane = Plane(tri);

        if (!plane.isFrontFacingTo(normalizedVelocity))
            continue; // we are going in the direction of the normal

        

        float t0, t1;
        bool embeddedInPlane = false;

        float signedDistToPlane = plane.signedDistanceTo(basePoint);


        float normalDotVelocity = glm::dot(plane.n, frameDelta);

        if (normalDotVelocity == 0.0f) // we are perpendicular to the plane
        {
            if (abs(signedDistToPlane) >= 1.0f) // not embedded in plane
            {
                continue;
            } else
            {
                // embedded in plane
                // we intersect it the whole time
                embeddedInPlane = true;
                t0 = 0;
                t1 = 1;
            }
        } else 
        {
            t0 = ( 1.0f - signedDistToPlane) / normalDotVelocity;
            t1 = (-1.0f - signedDistToPlane) / normalDotVelocity;

            if(t0 > t1) // swap them 
            {
                Util::swap(t0, t1);
            }

            if(t0 > 1.0f || t1 < 0.0f)
            {
                // both values are out of range [0, 1]
                continue;
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
        if(!embeddedInPlane)
        {
            glm::vec3 planeIntersectPoint = (basePoint - plane.n) + (t0 * frameDelta);

            if(Util::pointInTriangle(planeIntersectPoint, tri))
            {
                foundCollision = true;
                t = t0;
                collisionPoint = planeIntersectPoint;
            }
        }



        // swept sphere against points and edges
        // scary
        if(!foundCollision)
        {
            float velocityLength2 = Util::length2(frameDelta);

            float a, b, c; // quadratic equation
            float newT;

            // solve a*t² + b*t + c = 0
            a = velocityLength2;

            // check verts
            for (auto vert : tri.verts)
            {
                b = 2.0f * glm::dot(frameDelta, basePoint - vert);
                c = Util::length2(vert - basePoint) - 1.0f;
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
                glm::vec3 baseToVert = tri.verts[i] - basePoint;

                float edgeLength2 = Util::length2(edge);
                float edgeDotVelocity = glm::dot(edge, frameDelta);
                float edgeDotBaseToVert = glm::dot(edge, baseToVert);

                a = edgeLength2 * velocityLength2 + (edgeDotVelocity * edgeDotVelocity);
                b = edgeLength2 * (2.0f * glm::dot(frameDelta, baseToVert)) - 2.0f * edgeDotVelocity * edgeDotBaseToVert;
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

            // finally done!
            if(foundCollision)
            {
                // t = time of collision
                float distToCollision = t * glm::length(frameDelta);

                // is this the closest hit or nah
                if(distToCollision < closestCollision)
                {
                    closestCollision = distToCollision;
                    intersectionPoint = collisionPoint;
                    collided = foundCollision;
                    
                }
            }
        }




        if (collided) // TODO response
        {

            LOG_DEBUG("COLLIDING");


            // I suggest we add smart stuff under here



            // And above here

            // velocity = infinity;
            
            frameDelta = velocity * deltaTime;

            groundTri = &tri;
        }
    }
}

// TODO implement this function
// ReSharper disable once CppMemberFunctionMayBeStatic
void PlayerController::animatePlayer(Player& player)
{
    return;
}

void PlayerController::movePlayer(Player& player, float deltaTime) const
{
    // NAN check
    if (std::isnan(frameDelta.x))
        LOG_ERROR("frameDelta is NaN!");

    player.move(frameDelta);
}

bool PlayerController::isMoving() const
{
    return Util::length2(velocity) != 0.0f;
}