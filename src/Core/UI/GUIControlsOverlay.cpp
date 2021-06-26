#include "GUIControlsOverlay.h"
#include "Outrospection.h"
#include "UIButton.h"
#include "GUIScene.h"


void controlClick(UIButton& button, int)
{
    Outrospection& o = Outrospection::get();

    auto clickedControl = (Control)button.name[0]; // beauty of enum classes yay

    // change the keybind
    if (clickedControl != Control::NONE && !o.controlBound(clickedControl)) // can't rebind controls!
    {
        o.keyBinds.emplace_back(o.getEye(), clickedControl);

        button.name[button.name.length() - 1] = char(o.getEye());

        ((GUIScene*)o.scene)->pastPositions.clear(); // clear undo history
    }

    LOG("You pressed button %s", button.name);
}

GUIControlsOverlay::GUIControlsOverlay() : GUILayer("Controls Overlay", false),
                                           windowTitle("windowTitle", GL_LINEAR, 0.04, 0.1, 0.2, 0.05),
                                           windowBody("windowBody", GL_LINEAR, 0.04, 0.15, 0.2, 0.06),
                                           windowBottom("windowBottom", GL_LINEAR, 0.04, 0.21, 0.2, 0.0067)
{
}

void GUIControlsOverlay::tick()
{
    windowTitle.tick();
    windowBody.tick();
    windowBottom.tick();

    for (auto& button : buttons)
    {
        button->tick();
    }
}

void GUIControlsOverlay::draw() const
{
    windowTitle.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    windowBody.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    windowBottom.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

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
            buttonNames.emplace_back("L left N ");
            break;
        case 'R':
            buttonNames.emplace_back("R right N ");
            break;
        case '^':
            buttonNames.emplace_back("^ dash up N ");
            break;
        case '~':
            buttonNames.emplace_back("V dash down N ");
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

    float bodyHeight = 0.05 * buttons.size() + 0.02;
    windowBody.setScale(0.2, bodyHeight);
    windowBottom.setPosition(0.04, 0.15 + bodyHeight);
}
