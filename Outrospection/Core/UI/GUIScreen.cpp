#include "GUIScreen.h"

#include <utility>

#include "Source.h"
#include "Core/Rendering/Shader.h"

GUIScreen::GUIScreen(std::string _name, const bool _captureMouse)
	: name(std::move(_name)), captureMouse(_captureMouse)
{
	
}

void GUIScreen::onFocus()
{
	getOutrospection()->captureMouse(captureMouse);
}