#include "GUIControlsOverlay.h"
#include "Outrospection.h"
#include "UIButton.h"

void controlClick(UIButton& button, int)
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
    case 'R':
        theControlThatWasClicked = Control::MOVE_LEFT;
        break;
    case 'L':
        theControlThatWasClicked = Control::MOVE_RIGHT;
        break;
	case '^':
        theControlThatWasClicked = Control::DASH_UP;
        break;
    case '~':
        theControlThatWasClicked = Control::DASH_DOWN;
        break;
    case '<':
        theControlThatWasClicked = Control::DASH_LEFT;
        break;
    case '>':
        theControlThatWasClicked = Control::DASH_RIGHT;
        break;
	}
	
	// change the keybind
    if (theControlThatWasClicked != Control::NONE && !o.controlBound(theControlThatWasClicked)) { // can't rebind controls!
    	
        o.keyBinds.emplace_back(o.getEye(), theControlThatWasClicked);

        button.name[button.name.length() - 1] = char(o.getEye());
    }
	
    LOG("You pressed button %s", button.name);
}

GUIControlsOverlay::GUIControlsOverlay() : GUILayer("Controls Overlay", false),
	window("window", 0.04, 0.1, 0.2, 0.475)
{
    
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

void GUIControlsOverlay::setControls(const std::string& controlsStr)
{
    buttons.clear(); // TODO memory leak? need to free textures maybe?
    Outrospection::get().keyBinds.clear();
	
    std::vector<std::string> buttonNames;

    for (char c : controlsStr)
    {
        switch (c)
        {
        case 'U':
            buttonNames.emplace_back("U up N ");
            break;
        case 'D':
            buttonNames.emplace_back("D down N ");
            break;
        case 'L':
            buttonNames.emplace_back("L right N ");
            break;
        case 'R':
            buttonNames.emplace_back("R left N ");
            break;
        case '^':
            buttonNames.emplace_back("^ dash up N ");
            break;
        case '~':
            buttonNames.emplace_back("~ dash down N ");
            break;
        case '<':
            buttonNames.emplace_back("< dash left N ");
            break;
        case '>':
            buttonNames.emplace_back("> dash right N ");
            break;
        }
    }

    for (int i = 0; i < buttonNames.size(); i++)
    {
        buttons.emplace_back(std::make_unique<UIButton>(buttonNames[i],
            TextureManager::None,
            0.05, 0.16 + (0.05 * float(i)), 0.2, 0.05, Bounds(),
            controlClick));

        buttons[i]->showText = true;
    }
}
