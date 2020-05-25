#include "GUIScreen.h"

#include <utility>

#include "Core/Rendering/Shader.h"

GUIScreen::GUIScreen(std::string _name, std::vector<UIComponent> _elements)
	: name(std::move(_name)), elements(std::move(_elements))
{
	
}

void GUIScreen::draw(Shader& shader)
{
	for(const UIComponent& element : elements)
	{
		element.draw(shader);
	}
}
