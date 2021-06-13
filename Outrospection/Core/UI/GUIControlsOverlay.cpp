#include "GUIControlsOverlay.h"
#include "Outrospection.h"

void controlClick(UIButton& button, int mouseButton)
{
	
    Outrospection& o = Outrospection::get();

    Control theControlThatWasClicked = Control::NONE;
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
	
	// change the keybind
    if (theControlThatWasClicked != Control::NONE && !o.controlBound(theControlThatWasClicked)) { // can't rebind controls!
    	
        o.keyBinds.emplace_back(o.getEye(), theControlThatWasClicked);

        button.name[button.name.length() - 2] = char(o.getEye());
    }
	
    LOG("You pressed button %s", button.name);
}

GUIControlsOverlay::GUIControlsOverlay() : GUILayer("Controls Overlay", false),
	window("window", 0.05, 0.1, 0.15, 0.31)
{
    std::string buttonNames[] = {
        "Up [ ]",
        "Down [ ]",
        "Left [ ]",
        "Right [ ]"
    };

    for (int i = 0; i < sizeof(buttonNames) / sizeof(*buttonNames); i++)
    {
        buttons.emplace_back(std::make_unique<UIButton>(buttonNames[i],
            TextureManager::None,
            0.06, 0.15 + (0.05 * float(i)), 0.15, 0.05, Bounds(),
            controlClick));

    	// TODO show selected char in little box. Maybe font rendering?
        //buttons[i]->showText = true;
    }
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
    window.draw(Outrospection::get().spriteScanlinesShader, Outrospection::get().glyphShader);

    for (auto& button : buttons)
    {
        button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
}