#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "Core/Rendering/SimpleTexture.h"
#include "Core/Rendering/Resource.h"

enum class AnimType {
	walk,
	idle,
	jump, 
	fall
};

const std::unordered_map<AnimType, std::string> animTypeMap ({
	{AnimType::idle, "idle"},
	{AnimType::walk, "walk"},
	{AnimType::jump, "jump"},
	{AnimType::fall, "fall"}
});

//struct Animation {
//	AnimType animType;
//	unsigned int frameCount;
//	unsigned int frameLength;
//
//	unsigned int frameTally;
//
//	unsigned int curFrame;
//	std::vector<unsigned int> frames;
//};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;

	glm::vec2 texCoord;

	glm::vec3 tangent;
	glm::vec3 bitangent;
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
	glm::vec3 point;
	Triangle tri;
};

class Hashes
{
public:
	size_t operator() (const SimpleTexture& st) const
	{
		return st.texId;
	}

	size_t operator() (const Resource& r) const
	{
		std::hash<std::string> strhash;

		return strhash(r.resourcePath) + strhash(r.resourceName);
	}
};