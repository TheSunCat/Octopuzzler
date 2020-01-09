#pragma once

#include <glm/glm.hpp>
#include <GLAD\glad.h>
#include "stb_image.h"
#include <vector>
#include <sstream>
#include <iostream>
#include "Types.h"

std::vector<std::string> split(std::string input, std::string delimiter);

template <typename T>
void push_all(std::vector<T> &input, std::vector<T> &add) {
	for (T o : add)
		input.push_back(o);
}

glm::vec3 vecFromYaw(float yawDeg);

std::string vecToStr(glm::vec3 vec);

unsigned int TextureFromFile(const char* path, const std::string& directory);

unsigned char* DataFromFile(const char* path, const std::string& directory, int* widthOut, int* heightOut);

RayHit rayCast(
	const Ray& ray,
	const Triangle& tri, bool bothSides);

glm::vec3 rayCastPlane(Ray r, Triangle plane);

glm::vec3 getNormal(Triangle t);

float length2(glm::vec3 v);

inline bool zeroV3(glm::vec3 v)
{
	return (v.x == 0 && v.y == 0 && v.z == 0);
}