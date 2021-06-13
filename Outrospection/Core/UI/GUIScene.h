#pragma once
#include "Core/UI/GUILayer.h"
#include "Core/UI/UIComponent.h"

class GUIScene : public GUILayer
{
public:
	GUIScene(std::string& _level, int _rowLength);

	void tick() override;
	
	void draw() const override;

	std::string level; // stored in a grid like in Processing
	int rowLength;
	
	mutable UIComponent wall;
	mutable UIComponent hole;
	
	mutable UIComponent player; // oof. the player is a ui component. I need to find better names
};