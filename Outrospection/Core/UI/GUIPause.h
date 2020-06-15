#pragma once
#include "GUIScreen.h"
#include "UIButton.h"

class GUIPause :
    public GUIScreen
{
public:
	GUIPause();

	void tick() override;

	void draw(Shader& shader, Shader& glyphShader) const override;

private:
	UIButton buttonResume;
};
