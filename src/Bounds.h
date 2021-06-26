#pragma once

#include <vector>
#include <glm/vec2.hpp>

enum class BoundsShape
{
	None = 0,
	AABB = 1,
	Circle = 2,
};

class Bounds
{
public:
	Bounds(BoundsShape _shape, const std::vector<float>& boundsArgs);

	// assume default bounds
	Bounds();

	bool contains(const glm::vec2& point) const;
	
	BoundsShape shape;

	glm::vec2 pos;
	glm::vec2 size{};
	float radius2{};
};