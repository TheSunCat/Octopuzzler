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

void GUILayer::onEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<KeyPressedEvent>(std::bind(&GUILayer::onKeyPressed, this, std::placeholders::_1));
	dispatcher.dispatch<KeyReleasedEvent>(std::bind(&GUILayer::onKeyReleased, this, std::placeholders::_1));
}

bool GUILayer::onKeyPressed(KeyPressedEvent& event)
{
	return false;
}

bool GUILayer::onKeyReleased(KeyReleasedEvent& event)
{
	return false;
}