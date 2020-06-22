#include "Camera.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"

#include "Core/Scene.h"
#include "Core/World/Player.h"

Camera::Camera(const glm::vec3 _pos, const glm::vec3 _up, const float _yaw, const float _pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), rotationSpeed(ROT_SPEED), zoom(ZOOM)
{
	position = _pos;
	worldUp = _up;
	yaw = _yaw;
	pitch = _pitch;
	updateCameraVectors();
}

Camera::Camera(const float posX, const float posY, const float posZ, const float upX, const float upY, const float upZ, const float _yaw, const float _pitch)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), rotationSpeed(ROT_SPEED), zoom(ZOOM)
{
	position = glm::vec3(posX, posY, posZ);
	worldUp = glm::vec3(upX, upY, upZ);
	yaw = _yaw;
	pitch = _pitch;
	updateCameraVectors();
}

void Camera::calculateCameraPosition(const Player& player, const Scene& scene, const bool shouldAutoCam)
{
	if(framesSinceUserRotate < framesBeforeAutoCam)
		framesSinceUserRotate++;

	if(zoomVelocity != 0.0f)
	{
		if (desiredDistance != dist)
		{
			desiredDistance = Util::clamp(dist + zoomVelocity, 1.0f, maxDistance);
		}
		else
		{
			desiredDistance = Util::clamp(desiredDistance + zoomVelocity, 1.0f, maxDistance);
		}
		
		zoomVelocity *= zoomDrag;
		if (fabs(zoomVelocity) < 0.01f)
			zoomVelocity = 0.0f;
	}
	
	// what the camera is looking at
	const glm::vec3 newFocus = player.position + glm::vec3(0, player.eyeHeight, 0) + offset;
	focus = newFocus;

	// raycast backwards from the camera's focus
	std::array<Ray, 3> arRay = {
		Ray { focus - right * 0.1f, -front },
		Ray{ focus, -front },
		Ray { focus + right * 0.1f, -front } };

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
	
	if (closestDist < desiredDistance) // camera collision!
	{
		float correctedDist = closestDist * 0.9f;
													// first time we detect collision
		if (fabs(desiredDistance - dist) < 0.1f) // dist will change lots the next few times,
			framesRemainingLerpingDist = 10;		// so this won't trigger
													
		
		if (framesRemainingLerpingDist != 0) // lerp the transition to make it   s m o o t h
		{
			framesRemainingLerpingDist--;
			
			dist = Util::lerp(dist, correctedDist, 0.6f);
		}
		else // we're already colliding, let's just follow the wall
		{
			dist = correctedDist;
		}
	}
	else // not colliding, we're free to move all the way out to desiredDistance
	{
		if (desiredDistance - dist > 0.5f) // we're zooming out & it's a big diff so we want to avoid snap
			dist = Util::lerp(dist, desiredDistance, 0.12f);
		else
			dist = Util::lerp(dist, desiredDistance, 0.5f); // less snap
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

			Ray leftRay  = Ray{ focus, -Util::rotToVec3(yaw - float(30 * whiskersHit), pitch) };
			Ray rightRay = Ray{ focus, -Util::rotToVec3(yaw + float(30 * whiskersHit), pitch) };

			Collision leftCollision = Util::rayCast(leftRay, scene.wallCollision, false);
			collidedLeft = leftCollision.dist < desiredDistance;

			Collision rightCollision = Util::rayCast(rightRay, scene.wallCollision, false);
			collidedRight = rightCollision.dist < desiredDistance;

			whiskersHit++;
		} while (collidedLeft && collidedRight);

		float correctBy = 1.0f * float(whiskersHit + 1);

		if (collidedRight)
			yawVelocity -= correctBy;

		if (collidedLeft)
			yawVelocity += correctBy;
	}

	if (yawVelocity != 0.0f)
	{
		yaw += yawVelocity;

		yawVelocity *= yawDrag;
		if (fabs(yawVelocity) < 0.01f)
			yawVelocity = 0.0f;
	}

	if (pitchVelocity != 0.0f)
	{
		pitch += pitchVelocity;

		pitch = Util::clamp(pitch, -65.0f, 70.0f);

		pitchVelocity *= pitchDrag;
		if (fabs(pitchVelocity) < 0.01f)
			pitchVelocity = 0.0f;
	}
	
	updateCameraVectors();
	
	// get camera position
	const glm::vec3 newPos = focus - (front * dist);
	position = newPos;
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

void Camera::playerRotateCameraBy(float xoffset, float yoffset, const bool applyCameraSpeed)
{
	if (applyCameraSpeed)
	{
		xoffset *= rotationSpeed;
		yoffset *= rotationSpeed;
	}

	yawVelocity = xoffset;
	pitchVelocity = yoffset;

	if(fabs(xoffset) + fabs(yoffset) > 0.5f)
		framesSinceUserRotate = 0;
}

void Camera::changeDistBy(const float yoffset)
{
	zoomVelocity = yoffset / 3.0f;
}

void Camera::zoomBy(const float yoffset)
{
	zoom -= yoffset;

	zoom = Util::clamp(zoom, 1.0f, 45.0f);
}

void Camera::updateCameraVectors()
{
	front = Util::rotToVec3(yaw, pitch);
	// Also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}