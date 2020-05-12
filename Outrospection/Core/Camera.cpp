#include "Camera.h"

#include <array>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


#include "Source.h"
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
	if (getOutrospection()->controller.debugBreak)
	{
		std::cout << "DEBUG: break" << std::endl;
	}
	
	if(framesSinceUserRotate < framesBeforeAutoCam)
		framesSinceUserRotate++;
	
	offset = glm::vec3(0, 0.7, 0);
	
	// what the camera is looking at
	focus = player.position + offset;// glm::mix(focus, player.position + offset, 0.0725);

	Ray backRay = Ray{ focus, -front };
	Collision backRayCol = Util::rayCast(backRay, scene.collision, false);
	
	if (backRayCol.dist < desiredDistance)
	{
		dist = backRayCol.dist;
		focus += backRayCol.tri.n * 0.1f;// avoid clipping into wall
	}
	else
	{
		dist += dist >= desiredDistance ? 0.0f : 0.1f; // snap back to normal
	}
	
	if (framesSinceUserRotate >= framesBeforeAutoCam) {
		const unsigned int maxWhiskerCount = 10;

		// thanks to "50 Game Camera Mistakes" from GDC 2014
		// the 'whiskers' will point slightly offset of the camera so we know what's around us

		bool leftCol = false, rightCol = false;
		unsigned int i = 1;
		do
		{
			if (i >= maxWhiskerCount)
				break;

			Ray leftRay  = Ray{ focus, -Util::rotToVec3(yaw - (10.0f * i), pitch) };
			Ray rightRay = Ray{ focus, -Util::rotToVec3(yaw + (10.0f * i), pitch) };
			
			Collision leftCollision = Util::rayCast(leftRay, scene.collision, false);
			leftCol = leftCollision.dist < desiredDistance;
			
			Collision rightCollision = Util::rayCast(rightRay, scene.collision, false);
			rightCol = rightCollision.dist < desiredDistance;

			i++;
		} while (leftCol && rightCol);

		std::cout << i << ", left: " << leftCol << ", right: " << rightCol << std::endl;
		
		if(rightCol)
		{
			yaw -= 1.0f * (i + 1) / 5.0f;
		}

		if(leftCol)
		{
			yaw += 1.0f * (i + 1) / 5.0f;
		}
	}
	
	//std::cout << dist << std::endl;
	
	updateCameraVectors();
	
	// get camera position
	const glm::vec3 newPos = focus - (front * dist);
	
	position = newPos;
	
	
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

void Camera::playerRotateCameraBy(float xoffset, float yoffset, bool applyCameraSpeed, bool constrainPitch)
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

	if(fabs(xoffset) + fabs(yoffset) > 0.5f)
		framesSinceUserRotate = 0;
}

void Camera::zoomCameraBy(const float yoffset)
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