#pragma once

#include <glm/glm.hpp>

class Player;
class Scene;

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ROT_SPEED = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // automatically calculated, do not modify
    glm::vec3 mPosition{};
    glm::vec3 mFront;
    glm::vec3 mUp{};
    glm::vec3 mRight{};
    glm::vec3 mWorldUp{};

    // Euler angles
    float mYaw;
    float mPitch;

    // options
    glm::vec3 mFocus{};
    float mRotationSpeed;
    float mZoom;
    float mDesiredDistance = 5.0f;
    float maxDistance = mDesiredDistance;
    float mDist = mDesiredDistance;

    glm::vec3 mOffset{};

    explicit Camera(glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float _yaw = YAW, float _pitch = PITCH);

    // same ctor but w/ scalar args
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch);

    void calculateCameraPosition(const Player& player, const Scene& scene, float deltaTime, bool shouldAutoCam);

    glm::mat4 getViewMatrix() const;

    void playerRotateCameraBy(float xoffset, float yoffset, bool applyCameraSpeed = true);

    void changeDistBy(float yoffset);

    void zoomBy(float yoffset);
    void setDownVector(glm::vec3 vec);

private:
    // calculate front, right, and up vectors from camera angles
    void updateCameraVectors();

    unsigned int framesSinceUserRotate = 0;
    unsigned int framesBeforeAutoCam = 120;
    unsigned int framesRemainingLerpingDist = 0;

    float zoomVelocity = 0.0f;
    float zoomDrag = 0.8f;

    float yawVelocity = 0.0f;
    float yawDrag = 0.8f;

    float pitchVelocity = 0.0f;
    float pitchDrag = 0.9f;
};
