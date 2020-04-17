#pragma once

#include <GLAD/glad.h>
#include <glm/glm.hpp>

#include "Types.h"

std::vector<std::string> split(std::string input, std::string delimiter);

template <typename T>
void push_all(std::vector<T> &input, std::vector<T> &add) {
	for (T o : add)
		input.push_back(o);
}

glm::vec3 vecFromYaw(float yawDeg);

std::string vecToStr(const glm::vec3& vec);

unsigned char* DataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut);

RayHit rayCast(
	const Ray& ray,
	const Triangle& tri, bool bothSides);

glm::vec3 rayCastPlane(const Ray& r, const Triangle& plane);

glm::vec3 getNormal(const Triangle& t);

float length2V3(const glm::vec3& v);

bool isZeroV3(const glm::vec3& v);

float sumAbsV3(const glm::vec3& v);

float angleBetweenV3(const glm::vec3 a, const glm::vec3 b);

glm::vec3 projectV3(const glm::vec3 a, const glm::vec3 b);