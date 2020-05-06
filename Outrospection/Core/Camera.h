#pragma once

#include <vector>

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
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	
	// Euler angles
	float yaw;
	float pitch;

	// options
	float rotationSpeed;
	float zoom;
	glm::vec3 offset;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = YAW, float _pitch = PITCH);

	// same ctor but w/ scalar args
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	// PROBLEMATIC FUNCTION (requires the two includes)
	void calculateCameraPosition(const Player& player, const Scene& scene);

	glm::mat4 getViewMatrix();

	void rotateCameraBy(float xoffset, float yoffset, bool applyCameraSpeed = true, bool constrainPitch = true);

	void zoomCameraBy(float yoffset);

private:
	// calculate the camera vectors from the updated Euler angles
	void updateCameraVectors();
};