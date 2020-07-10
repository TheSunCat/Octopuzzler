#pragma once
#include <functional>

#include "Core/UI/UIComponent.h"

class UIButton :
    public UIComponent
{
public:
	UIButton(const std::string& _texName, float posXPercent, float posYPercent, float widthPercent, float heightPercent,
	         std::function<void()> clickCallback = nullptr);

	bool isOnButton(const glm::vec2& point) const;
	
	void tick() override;

	std::function<void()> onClick;
	
	bool hovered = false;
};