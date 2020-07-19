#pragma once
#include <string>

#include "UIComponent.h"
#include "Core/Layer.h"
#include "Events/KeyEvent.h"

class GUILayer : public Layer
{
public:
    explicit GUILayer(std::string _name, bool _captureMouse = true);

    void onAttach() override;

    void onEvent(Event& event) override;

    virtual bool onKeyPressed(KeyPressedEvent& event);
    virtual bool onKeyReleased(KeyReleasedEvent& event);
private:
    std::string name;
    bool captureMouse = false;
};
