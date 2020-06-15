#pragma once
#include "GUIScreen.h"

class GUIIngame :
    public GUIScreen
{
public:
	GUIIngame();

	void tick() override;

	void draw(Shader& shader, Shader& glyphShader) const override;

	DISALLOW_COPY_AND_ASSIGN(GUIIngame)
private:
	UIComponent healthDisplay;
	
	UIComponent posX;
	UIComponent posY;
	UIComponent posZ;
};