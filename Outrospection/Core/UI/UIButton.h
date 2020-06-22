#pragma once
#include <functional>

#include "Core/UI/UIComponent.h"

class UIButton :
    public UIComponent
{
public:
	UIButton(const std::string& _texName, float posXPercent, float posYPercent, float widthPercent, float heightPercent,
	         std::function<void()> clickCallback = nullptr);
	
	void tick() override;
	
private:
	bool hovered = false;
	
	std::function<void()> onClick;
};