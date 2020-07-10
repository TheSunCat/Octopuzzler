#pragma once
#include "GUILayer.h"

class GUIIngame :
    public GUILayer
{
public:
	GUIIngame();

	void tick() override;

	void draw() const override;

	DISALLOW_COPY_AND_ASSIGN(GUIIngame)
private:
	UIComponent healthDisplay;
	
	UIComponent posX;
	UIComponent posY;
	UIComponent posZ;
};