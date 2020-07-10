#include "GUILayer.h"

#include <utility>

#include "Outrospection.h"
#include "Core/Rendering/Shader.h"

GUILayer::GUILayer(std::string _name, const bool _captureMouse) : Layer(),
    name(std::move(_name)), captureMouse(_captureMouse)
{
	
}

void GUILayer::onAttach()
{
	Outrospection::get().captureMouse(captureMouse);
}