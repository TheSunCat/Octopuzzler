#pragma once
#include <string>

#include "UIComponent.h"

class GUIScreen
{
public:
	explicit GUIScreen(std::string _name, bool _captureMouse = true);

	virtual void onFocus();
	
	virtual void draw(Shader& shader, Shader& glyphShader) const = 0;

	virtual void tick() = 0;

	virtual ~GUIScreen() = default;
private:
	std::string name;
	bool captureMouse = false;
};