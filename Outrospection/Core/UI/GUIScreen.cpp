#include "GUIScreen.h"

#include <utility>

#include "Source.h"
#include "Core/Rendering/Shader.h"

GUIScreen::GUIScreen(std::string _name, std::vector<UIComponent> _elements, const bool _captureMouse)
	: name(std::move(_name)), elements(std::move(_elements)), captureMouse(_captureMouse)
{
	
}

void GUIScreen::onFocus()
{
	getOutrospection()->captureMouse(captureMouse);
}

void GUIScreen::draw(Shader& shader, Shader& glyphShader) const
{
	for(const UIComponent& element : elements)
	{
		element.draw(shader, glyphShader);
	}
}

void GUIScreen::tick()
{
	for(UIComponent& element : elements)
	{
		element.tick();
	}
}