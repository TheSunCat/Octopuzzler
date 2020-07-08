#include "GUIScreen.h"

#include <utility>

#include "Outrospection.h"
#include "Core/Rendering/Shader.h"

GUIScreen::GUIScreen(std::string _name, const bool _captureMouse)
	: name(std::move(_name)), captureMouse(_captureMouse)
{
	
}

void GUIScreen::onFocus()
{
	Outrospection::get().captureMouse(captureMouse);
}