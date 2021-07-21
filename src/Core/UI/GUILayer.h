#pragma once
#include <string>

#include "Core/Layer.h"

class UIButton;
class KeyPressedEvent;
class KeyReleasedEvent;
class MouseButtonPressedEvent;

class GUILayer : public Layer
{
public:
    explicit GUILayer(const std::string& _name, bool _captureMouse = true);

    void onAttach() override;
    void onDetach() override;

    void onEvent(Event& event) override;

    virtual bool onKeyPressed(KeyPressedEvent& event);
    virtual bool onKeyReleased(KeyReleasedEvent& event);
    virtual bool onMousePressed(MouseButtonPressedEvent& event);

protected:
    std::vector<std::unique_ptr<UIButton>> buttons;
    bool captureMouse = false;
private:
    std::string name;
};
