#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "Core/Rendering/SimpleTexture.h"
#include "Core/Rendering/Resource.h"

//enum class AnimType {
//	walk,
//	idle,
//	jump, 
//	fall
//};
//
//const std::unordered_map<AnimType, std::string> animTypeMap ({
//	{AnimType::idle, "idle"},
//	{AnimType::walk, "walk"},
//	{AnimType::jump, "jump"},
//	{AnimType::fall, "fall"}
//});

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;

	glm::vec2 texCoord;

	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct Triangle
{
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 n;
};

struct Ray
{
	glm::vec3 origin; // where the ray starts
	glm::vec3 direction; // direction the ray is going in
};

struct Collision
{
	float dist;
	glm::vec3 point;
	Triangle tri;
	glm::vec3 shiftBy;
};

struct FontCharacter
{
	GLuint textureId;
	glm::ivec2 size;
	glm::ivec2 bearing; // offset from base line
	unsigned int advance; // offset to advance to next glyph
};

typedef glm::vec3 Color;

class Hashes
{
public:
	size_t operator() (const SimpleTexture& st) const
	{
		return st.texId;
	}

	size_t operator() (const Resource& r) const
	{
		const std::hash<std::string> strhash;

		return strhash(r.fullPath);
	}
};