#pragma once
#include <string>

enum AnimType {
	walk,
	idle,
	jump
};

struct Animation {
	AnimType animType;
	unsigned int frameCount;
	unsigned int frameLength;

	unsigned int frameTally;

	unsigned int curFrame;
	std::vector<unsigned int> frames;
};

struct Triangle {
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 n;
};

struct Ray {
	glm::vec3 origin; // where the ray starts
	glm::vec3 direction; // direction the ray is going in
};

struct RayHit {
	float dist;
	glm::vec3 hitPoint;
};