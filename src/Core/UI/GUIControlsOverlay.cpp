#include "GUIControlsOverlay.h"
#include "Outrospection.h"
#include "UIButton.h"
#include "GUIScene.h"


void controlClick(UIButton& button, int)
{
    auto scene = (GUIScene*)Outrospection::get().scene;

    auto clickedControl = (Control)button.name[0]; // beauty of enum classes yay

    // change the keybind
    if (clickedControl != Control::NONE && !scene->controlBound(clickedControl)) // can't rebind controls!
    {
        scene->keyBinds.emplace_back(Outrospection::get().getEye(), clickedControl);

        button.name[button.name.length() - 1] = char(Outrospection::get().getEye());

        scene->pastPositions.clear(); // clear undo history
    }

    LOG("You pressed button %s", button.name);
}

GUIControlsOverlay::GUIControlsOverlay() : GUILayer("Controls Overlay", false),
                                           windowTitle("windowTitle", GL_LINEAR, UITransform(38, 162, 384, 54)),
                                           windowBody("windowBody", GL_LINEAR, UITransform(38, 216, 384, 65)),
                                           windowBottom("windowBottom", GL_LINEAR, UITransform(38, 281, 384, 7))
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
    ((GUIScene*)Outrospection::get().scene)->keyBinds.clear();

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
                                                        UITransform(45, 227 + (54 * i), 384, 54), Bounds(),
                                                        controlClick));

        buttons[i]->showText = true;
    }

    int bodyHeight = 54 * buttons.size() + 22;
    windowBody.setScale(38, bodyHeight);
    windowBottom.setPosition(38, 216 + bodyHeight);
}
