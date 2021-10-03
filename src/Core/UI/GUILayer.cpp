#include "GUILayer.h"

#include <utility>

#include "Outrospection.h"
#include "UIButton.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

GUILayer::GUILayer(const std::string& _name, const bool _captureMouse) : Layer(),
                captureMouse(_captureMouse), name(_name)
{
}

void GUILayer::onAttach()
{
    Outrospection::get().captureMouse(captureMouse);
    LOG("Attached %s", name);
}

void GUILayer::onDetach()
{
    Outrospection::get().captureMouse(!captureMouse);
    LOG("Detached %s", name);
}

void GUILayer::onEvent(Event& event)
{
    dispatchEvent<KeyPressedEvent>(event, std::bind(&GUILayer::onKeyPressed, this, std::placeholders::_1));
    dispatchEvent<KeyReleasedEvent>(event, std::bind(&GUILayer::onKeyReleased, this, std::placeholders::_1));
    dispatchEvent<MouseButtonPressedEvent>(event, std::bind(&GUILayer::onMousePressed, this, std::placeholders::_1));
}

bool GUILayer::onKeyPressed(KeyPressedEvent& event)
{
    return false;
}

bool GUILayer::onKeyReleased(KeyReleasedEvent& event)
{
    return false;
}

bool GUILayer::onMousePressed(MouseButtonPressedEvent& event)
{
    for (auto& button : buttons)
    {
        if (button->hovered)
        {
            button->onClick(*button, event.getMouseButton()); // TODO feed mouse coords maybe

            return true; // handled
        }
    }

    // TODO this should be elsewhere. where?
    Outrospection::get().setEye(Eye::NONE);

    return false;
}
