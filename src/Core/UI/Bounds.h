#pragma once

#include <vector>
#include <glm/vec2.hpp>

#include "UIComponent.h"

enum class BoundsShape
{
	None = 0,
	AABB = 1,
	Circle = 2,
};

class Bounds
{
public:
	Bounds(const UITransform& transform, BoundsShape _shape = BoundsShape::AABB);

	// assume default bounds
	Bounds();

	bool contains(const glm::vec2& point) const;
	
	BoundsShape shape;

	UITransform transform;
};