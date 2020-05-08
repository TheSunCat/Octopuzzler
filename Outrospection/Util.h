#pragma once

#include <glm/glm.hpp>

#include "Types.h"

namespace Util
{
	// Check for OpenGL errors and print them
	bool glError(bool print);

	void split(const std::string& input, const char& delimiter, std::vector<std::string>& out);

	template <typename T>
	void push_all(std::vector<T>& input, std::vector<T>& add) {
		for (const T& o : add)
			input.emplace_back(o);
	}

	glm::vec3 rotToVec3(float yaw, float pitch = 0);

	std::string vecToStr(const glm::vec3& vec);

	unsigned char* DataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut);

	RayHit rayCast(
		const Ray& ray,
		const Triangle& tri, bool bothSides);

	glm::vec3 rayCastPlane(const Ray& r, const Triangle& plane);

	RayHit rayCast(const Ray& r, const std::vector<Triangle>& tris, bool bothSides = false);

	bool intersectRaySegmentSphere(const ::Ray& ray, const glm::vec3 sphereOrigin, const float sphereRadius2, glm::vec3& ip);

	// 2D test for which side of a 2D line a 2D point lies on
	bool leftOf(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p);

	bool pointInside(const glm::vec2 poly[], const int pcount, const glm::vec2& v);

	glm::vec3 getNormal(const Triangle& t);

	float length2V3(const glm::vec3& v);

	bool isZeroV3(const glm::vec3& v);

	float sumAbsV3(const glm::vec3& v);

	float angleBetweenV3(const glm::vec3 a, const glm::vec3 b);

	glm::vec3 projectV3(const glm::vec3 a, const glm::vec3 b);

	// return input depending on constant deadzones and limitzones
	float valFromJoystickAxis(float axis);
}
