#pragma once
#include <string>
#include <vector>

#include "UIComponent.h"

class GUIScreen
{
public:
	GUIScreen(const std::string& _name);
	
	GUIScreen(std::string _name, std::vector<UIComponent> _elements);

	virtual void draw(Shader& shader, Shader& glyphShader) const;

	virtual void tick();

	virtual ~GUIScreen() = default;
private:
	std::string name;
	std::vector<UIComponent> elements;
};
