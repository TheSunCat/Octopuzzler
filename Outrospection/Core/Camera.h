#pragma once

#include <glm/glm.hpp>

class Player;
class Scene;

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
//const float SPEED = 2.5f * 2;
const float ROT_SPEED = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	// automatically calculated, do not modify
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	
	// Euler angles
	float yaw;
	float pitch;

	// options
	glm::vec3 focus;
	float rotationSpeed;
	float zoom;
	float desiredDistance = 5.0f;
	float maxDistance = desiredDistance;
	float dist = desiredDistance;
	
	glm::vec3 offset;

	Camera(glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f),
	       float _yaw = YAW, float _pitch = PITCH);

	// same ctor but w/ scalar args
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	void calculateCameraPosition(const Player& player, const Scene& scene, bool shouldAutoCam);

	glm::mat4 getViewMatrix() const;

	void playerRotateCameraBy(float xoffset, float yoffset, bool applyCameraSpeed = true, bool constrainPitch = true);

	void changeDistBy(float yoffset);
	
	void zoomBy(float yoffset);

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