#include "GUIScreen.h"

#include <utility>

#include "Core/Rendering/Shader.h"

GUIScreen::GUIScreen(std::string _name, std::vector<UIComponent> _elements)
	: name(std::move(_name)), elements(std::move(_elements))
{
	
}

void GUIScreen::draw(Shader& shader) const
{
	for(const UIComponent& element : elements)
	{
		element.draw(shader);
	}
}

void GUIScreen::tick()
{
	for(UIComponent& element : elements)
	{
		element.tick();
	}
}