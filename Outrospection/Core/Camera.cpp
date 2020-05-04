#include "Camera.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = position;
	worldUp = up;
	yaw = yaw;
	pitch = pitch;
	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = glm::vec3(posX, posY, posZ);
	worldUp = glm::vec3(upX, upY, upZ);
	yaw = _yaw;
	pitch = _pitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

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

void Camera::processMouseScroll(float yoffset)
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
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}