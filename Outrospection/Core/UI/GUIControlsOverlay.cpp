#include "GUIControlsOverlay.h"
#include "Outrospection.h"

void controlClick(UIButton& button, int mouseButton)
{
    LOG("You pressed button %s", button.name);
	
    Outrospection& o = Outrospection::get();

    Control theControlThatWasClicked;
	switch(button.name[0])
	{
    case 'U':
        theControlThatWasClicked = Control::MOVE_UP;
        break;
    case 'D':
        theControlThatWasClicked = Control::MOVE_DOWN;
        break;
    case 'L':
        theControlThatWasClicked = Control::MOVE_LEFT;
        break;
    case 'R':
        theControlThatWasClicked = Control::MOVE_RIGHT;
        break;
	}
	
    if (true) { //(mouseButton == GLFW_MOUSE_BUTTON_LEFT) {// change the keybind
        o.keyBinds.emplace_back(o.getEye(), theControlThatWasClicked);

        button.name[button.name.length() - 2] = char(o.getEye());
    }
}

GUIControlsOverlay::GUIControlsOverlay() : GUILayer("Controls Overlay", false),
	window("binbows", 0.05, 0.1, 0.3, 0.41)
{
    buttons.emplace_back(std::make_unique<UIButton>("Up [ ]",
        Outrospection::get().textureManager.loadTexture({ "UI/", "button" }),
        0.06, 0.175, 0.225, 0.075, Bounds(),
        controlClick));
    buttons[0]->showText = true;

    buttons.emplace_back(std::make_unique<UIButton>("Down [ ]",
        Outrospection::get().textureManager.loadTexture({ "UI/", "button" }),
        0.06, 0.255, 0.225, 0.075, Bounds(),
        controlClick));
    buttons[1]->showText = true;

    buttons.emplace_back(std::make_unique<UIButton>("Left [ ]",
        Outrospection::get().textureManager.loadTexture({ "UI/", "button" }),
        0.06, 0.335, 0.225, 0.075, Bounds(),
        controlClick));
    buttons[2]->showText = true;
	
    buttons.emplace_back(std::make_unique<UIButton>("Right [ ]",
        Outrospection::get().textureManager.loadTexture({ "UI/", "button" }),
        0.06, 0.415, 0.225, 0.075, Bounds(),
        controlClick));
    buttons[3]->showText = true;
}

void GUIControlsOverlay::tick()
{
    window.tick();

    for (auto& button : buttons)
    {
        button->tick();
    }
}

void GUIControlsOverlay::draw() const
{
    window.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

    for (auto& button : buttons)
    {
        button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
}