#include "GUIMenu.h"

#include <utility>

#include "Core/Rendering/Shader.h"

GUIMenu::GUIMenu(std::string _name, std::vector<UIComponent> _elements)
	: name(std::move(_name)), elements(std::move(_elements))
{
	
}

void GUIMenu::draw(Shader& shader)
{
	for(const UIComponent& element : elements)
	{
		element.draw(shader);
	}
}
