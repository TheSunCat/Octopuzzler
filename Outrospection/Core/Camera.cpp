#include "Camera.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"

#include "Core/Scene.h"
#include "Core/World/Player.h"

Camera::Camera(const glm::vec3 _pos, const glm::vec3 _up, const float _yaw, const float _pitch)
    : mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mRotationSpeed(ROT_SPEED), mZoom(ZOOM)
{
    mPosition = _pos;
    mWorldUp = _up;
    mYaw = _yaw;
    mPitch = _pitch;
    updateCameraVectors();
}

Camera::Camera(const float posX, const float posY, const float posZ, const float upX, const float upY, const float upZ,
               const float _yaw, const float _pitch)
    : mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mRotationSpeed(ROT_SPEED), mZoom(ZOOM)
{
    mPosition = glm::vec3(posX, posY, posZ);
    mWorldUp = glm::vec3(upX, upY, upZ);
    mYaw = _yaw;
    mPitch = _pitch;
    updateCameraVectors();
}

void Camera::pan_ahead_of_player(const Player& player) {
    
}

void Camera::calculateCameraPosition(const Player& player, const Scene& scene, const bool shouldAutoCam)
{
    if (framesSinceUserRotate < framesBeforeAutoCam)
        framesSinceUserRotate++;

    if (zoomVelocity != 0.0f)
    {
        if (mDesiredDistance != mDist)
        {
            mDesiredDistance = Util::clamp(mDist + zoomVelocity, 1.0f, maxDistance);
        }
        else
        {
            mDesiredDistance = Util::clamp(mDesiredDistance + zoomVelocity, 1.0f, maxDistance);
        }

        zoomVelocity *= zoomDrag;
        if (fabs(zoomVelocity) < 0.01f)
            zoomVelocity = 0.0f;
    }

    // what the camera is looking at
    const glm::vec3 newFocus = player.position + glm::vec3(0, player.eyeHeight, 0) + mOffset;
    mFocus = glm::mix(mFocus, newFocus, 0.5f);

    // raycast backwards from the camera's focus
    std::array<Ray, 3> arRay = {
        Ray{mFocus - mRight * 0.1f, -mFront},
        Ray{mFocus, -mFront},
        Ray{mFocus + mRight * 0.1f, -mFront}
    };

    float closestDist = INFINITY;
    for (unsigned int i = 0; i < arRay.size(); i++)
    {
        Collision curCollision = Util::rayCast(arRay[i], scene.collision, false);

        // i = 0 makes this 0.8, i = 1 makes this 1, i = 2 makes this 0.8
        const float sideMultiplier = 1 - (abs(i - 1.0f) * 0.2f);

        const float curDist = curCollision.dist * sideMultiplier;

        if (curDist < closestDist)
            closestDist = curDist;
    }

    if (closestDist < mDesiredDistance) // camera collision!
    {
        float correctedDist = closestDist * 0.9f;
                                                    // first time we detect collision
        if (fabs(mDesiredDistance - mDist) < 0.1f) // dist will change lots the next few times,
            framesRemainingLerpingDist = 10;        // so this won't trigger
                                                    
        
        if (framesRemainingLerpingDist != 0) // lerp the transition to make it   s m o o t h
        {
            framesRemainingLerpingDist--;

            mDist = Util::lerp(mDist, correctedDist, 0.6f);
        }
        else // we're already colliding, let's just follow the wall
        {
            mDist = correctedDist;
        }
    }
    else // not colliding, we're free to move all the way out to desiredDistance
    {
        if (mDesiredDistance - mDist > 0.5f) // we're zooming out & it's a big diff so we want to avoid snap
            mDist = Util::lerp(mDist, mDesiredDistance, 0.12f);
        else
            mDist = Util::lerp(mDist, mDesiredDistance, 0.5f); // less snap
    }

    bool autoCamming = (framesSinceUserRotate >= framesBeforeAutoCam) && shouldAutoCam;

    if (autoCamming) { // rotate camera to avoid occluding the player
        const unsigned int maxWhiskerCount = 10;

        // thanks to "50 Game Camera Mistakes" from GDC 2014
        // the 'whiskers' will point slightly offset of the camera so we know what's around us
        bool collidedLeft = false, collidedRight = false;
        unsigned int whiskersHit = 1;
        do
        {
            if (whiskersHit >= maxWhiskerCount)
                break;

            Ray leftRay = Ray{mFocus, -Util::rotToVec3(mYaw - float(30 * whiskersHit), mPitch)};
            Ray rightRay = Ray{mFocus, -Util::rotToVec3(mYaw + float(30 * whiskersHit), mPitch)};

            Collision leftCollision = Util::rayCast(leftRay, scene.wallCollision, false);
            collidedLeft = leftCollision.dist < mDesiredDistance;

            Collision rightCollision = Util::rayCast(rightRay, scene.wallCollision, false);
            collidedRight = rightCollision.dist < mDesiredDistance;

            whiskersHit++;
        }
        while (collidedLeft && collidedRight);

        float correctBy = 1.0f * float(whiskersHit + 1);

        if (collidedRight)
            yawVelocity -= correctBy;

        if (collidedLeft)
            yawVelocity += correctBy;
    }

    if (yawVelocity != 0.0f)
    {
        mYaw += yawVelocity;

        yawVelocity *= yawDrag;
        if (fabs(yawVelocity) < 0.01f)
            yawVelocity = 0.0f;
    }

    if (pitchVelocity != 0.0f)
    {
        mPitch += pitchVelocity;

        mPitch = Util::clamp(mPitch, -65.0f, 70.0f);

        pitchVelocity *= pitchDrag;
        if (fabs(pitchVelocity) < 0.01f)
            pitchVelocity = 0.0f;
    }

    updateCameraVectors();

    // get camera position
    const glm::vec3 newPos = mFocus - (mFront * mDist);
    mPosition = newPos;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::playerRotateCameraBy(float xoffset, float yoffset, const bool applyCameraSpeed)
{
    if (applyCameraSpeed)
    {
        xoffset *= mRotationSpeed;
        yoffset *= mRotationSpeed;
    }

    yawVelocity = xoffset;
    pitchVelocity = yoffset;

    if (fabs(xoffset) + fabs(yoffset) > 0.5f)
        framesSinceUserRotate = 0;
}

void Camera::changeDistBy(const float yoffset)
{
    zoomVelocity = yoffset / 3.0f;
}

void Camera::zoomBy(const float yoffset)
{
    mZoom -= yoffset;

    mZoom = Util::clamp(mZoom, 1.0f, 45.0f);
}

void Camera::updateCameraVectors()
{
    mFront = Util::rotToVec3(mYaw, mPitch);
    // Also re-calculate the Right and Up vector
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    mUp = glm::normalize(glm::cross(mRight, mFront));
}
