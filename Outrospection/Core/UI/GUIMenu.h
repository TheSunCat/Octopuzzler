#pragma once
#include <string>
#include <vector>

#include "UIComponent.h"

class GUIMenu
{
public:
	GUIMenu(const std::string& _name);
	
	GUIMenu(std::string _name, std::vector<UIComponent> _elements);

	void draw(Shader& shader);

private:
	std::string name;
	std::vector<UIComponent> elements;
};
