#include "GUIControlsOverlay.h"
#include "Outrospection.h"
#include "UIButton.h"
#include "GUIScene.h"


void controlClick(UIButton& button, int)
{
    LOG("You pressed button %s", button.name);

    auto scene = (GUIScene*)Outrospection::get().scene;

    auto clickedControl = (Control)button.name[0]; // beauty of enum classes yay

    // change the keybind
    if (clickedControl != Control::NONE && !scene->controlBound(clickedControl)) // can't rebind controls!
    {
        Outrospection::get().audioManager.play("Control_Select");

        scene->keyBinds.emplace_back(Outrospection::get().getEye(), clickedControl);

        button.name[button.name.length() - 1] = char(Outrospection::get().getEye());

        scene->pastPositions.clear(); // clear undo history
    }
}

GUIControlsOverlay::GUIControlsOverlay() : GUILayer("Controls Overlay", false),
                                           windowTitle("windowTitle", GL_LINEAR, 0.02, 0.15, 0.2, 0.05),
                                           windowBody("windowBody", GL_LINEAR, 0.02, 0.20, 0.2, 0.06),
                                           windowBottom("windowBottom", GL_LINEAR, 0.02, 0.26, 0.2, 0.0067)
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
                                                        0.03, 0.21 + (0.05 * float(i)), 0.2, 0.05, Bounds(),
                                                        controlClick));

        buttons[i]->showText = true;
    }

    float bodyHeight = 0.05 * buttons.size() + 0.02;
    windowBody.setScale(0.2, bodyHeight);
    windowBottom.setPosition(0.021f, 0.20f + bodyHeight); // 0.021f because 0.02 was misaligned for some reason aaaaa
}
