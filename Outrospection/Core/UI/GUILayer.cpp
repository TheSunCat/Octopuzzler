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
    dispatchEvent<KeyPressedEvent>(event, std::bind(&GUILayer::onKeyPressed, this, std::placeholders::_1));
    dispatchEvent<KeyReleasedEvent>(event, std::bind(&GUILayer::onKeyReleased, this, std::placeholders::_1));
}

bool GUILayer::onKeyPressed(KeyPressedEvent& event)
{
    return false;
}

bool GUILayer::onKeyReleased(KeyReleasedEvent& event)
{
    return false;
}