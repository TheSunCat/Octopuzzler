#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"

#include "Core/Player.h"
#include "Core/Scene.h"

Camera::Camera(glm::vec3 _pos, glm::vec3 _up, float _yaw, float _pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), rotationSpeed(ROT_SPEED), zoom(ZOOM)
{
	position = _pos;
	worldUp = _up;
	yaw = _yaw;
	pitch = _pitch;
	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), rotationSpeed(ROT_SPEED), zoom(ZOOM)
{
	position = glm::vec3(posX, posY, posZ);
	worldUp = glm::vec3(upX, upY, upZ);
	yaw = _yaw;
	pitch = _pitch;
	updateCameraVectors();
}

void Camera::calculateCameraPosition(const Player& player, const Scene& scene)
{
	// what the camera is looking at
	const glm::vec3 cameraFocus = position + offset;

	Ray backRay = Ray{ cameraFocus, -front };

//	Ray whiskers[5] = { firstRay, backRay, lastRay };


	// thanks to "50 Game Camera Mistakes" from GDC 2014

}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

void Camera::rotateCameraBy(float xoffset, float yoffset, bool applyCameraSpeed, bool constrainPitch)
{
	if (applyCameraSpeed)
	{
		xoffset *= rotationSpeed;
		yoffset *= rotationSpeed;
	}

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::zoomCameraBy(float yoffset)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= yoffset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	front = Util::rotToVec3(yaw, pitch);
	// Also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}