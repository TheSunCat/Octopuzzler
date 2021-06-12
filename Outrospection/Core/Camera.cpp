#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"

Camera::Camera(const glm::vec3 _pos, const glm::vec3 _up, const float _yaw, const float _pitch, const float screenWidth, const float screenHeight)
    : Camera(_pos.x, _pos.y, _pos.z, _up.x, _up.y, _up.z, _yaw, _pitch, screenWidth, screenHeight)
{
    
}

Camera::Camera(const float posX, const float posY, const float posZ, const float upX, const float upY, const float upZ,
               const float _yaw, const float _pitch, const float screenWidth, const float screenHeight)
    : mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mRotationSpeed(ROT_SPEED), mZoom(ZOOM), screenSize(screenWidth, screenHeight)
{
    mPosition = glm::vec3(posX, posY, posZ);
    mWorldUp = glm::vec3(upX, upY, upZ);
    mYaw = _yaw;
    mPitch = _pitch;
    updateCameraData();
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

void Camera::setDownVector(glm::vec3 vec)
{
    mWorldUp = -vec;
}

void Camera::updateCameraData()
{
    mFront = Util::rotToVec3(mYaw, mPitch);
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));

    // calculate matrices
    proj = glm::perspective(glm::radians(mZoom), float(screenSize.x) / float(screenSize.y),
        0.1f, 1000.0f); // zNear and zFar

    view = glm::lookAt(mPosition, mPosition + mFront, mUp);

    viewProj = proj * view;

    // store frustum
    Util::getFrustumFromViewProj(viewProj, frustum);
}
