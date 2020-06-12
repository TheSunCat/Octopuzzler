#pragma once
#include <string>
#include <vector>

#include "UIComponent.h"

class GUIScreen
{
public:
	explicit GUIScreen(std::string _name, std::vector<UIComponent> _elements = {}, bool _captureMouse = true);

	virtual void onFocus();
	
	virtual void draw(Shader& shader, Shader& glyphShader) const;

	virtual void tick();

	virtual ~GUIScreen() = default;
private:
	std::string name;
	std::vector<UIComponent> elements;
	bool captureMouse;
};
