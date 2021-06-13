#pragma once
#include "Core/UI/GUILayer.h"
#include "Core/UI/UIComponent.h"

class GUIScene : public GUILayer
{
public:
	GUIScene(Level& _level);

	void tick() override;
	
	void draw() const override;

	void tryMovePlayer(Control input);

	Level level;
	mutable glm::vec2 playerPos; // interpolates player between grid spots
	mutable glm::vec2 playerPosInt; // actual position on grid
	
	mutable UIComponent wall;
	mutable UIComponent hole;
	
	mutable UIComponent playerSprite;
};