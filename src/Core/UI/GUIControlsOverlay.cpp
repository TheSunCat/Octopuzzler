#include "GUIControlsOverlay.h"
#include "Core/UI/UIComponent.h"
#include "Outrospection.h"
#include "UIButton.h"
#include "GUIScene.h"


void controlClick(UIButton& button, int)
{
    LOG("You pressed button %s", button.text);

    auto scene = (GUIScene*)Outrospection::get().scene;

    auto clickedControl = (Control)button.text[0]; // beauty of enum classes yay

    // change the keybind
    if (clickedControl != Control::NONE && !scene->controlBound(clickedControl)) // can't rebind controls!
    {
        Outrospection::get().audioManager.play("Control_Select");

        scene->keyBinds.emplace_back(Outrospection::get().getEye(), clickedControl);

        button.text[button.text.length() - 1] = char(Outrospection::get().getEye());

        scene->pastPositions.clear(); // clear undo history
    }
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

    currentBodyHeight = Util::lerp(currentBodyHeight, bodyHeight, 0.5);
    windowBody.setScale(384, currentBodyHeight);
    windowBottom.setPosition(38, 216 + currentBodyHeight);

    if (bodyHeight != 0) {
        for (auto& button : buttons)
        {
            button->tick();
        }
    }
}

void GUIControlsOverlay::draw() const
{
    windowTitle.draw();
    windowBody.draw();
    windowBottom.draw();

    if (bodyHeight != 0) {
        for (auto& button : buttons)
        {
            button->draw();
        }
    }
}

void GUIControlsOverlay::setControls(const std::string& controlsStr)
{
    buttons.clear();
    ((GUIScene*)Outrospection::get().scene)->keyBinds.clear();

    std::vector<std::string> buttonNames;

    for (char c : controlsStr)
    {
        switch (Control(c))
        {
        case Control::MOVE_UP:
            buttonNames.emplace_back("* up # ");
            break;
        case Control::MOVE_DOWN:
            buttonNames.emplace_back(", down # ");
            break;
        case Control::MOVE_LEFT:
            buttonNames.emplace_back("( left # ");
            break;
        case Control::MOVE_RIGHT:
            buttonNames.emplace_back(") right # ");
            break;
        case Control::DASH_UP:
            buttonNames.emplace_back("^ dash up # ");
            break;
        case Control::DASH_DOWN:
            buttonNames.emplace_back("_ dash down # ");
            break;
        case Control::DASH_LEFT:
            buttonNames.emplace_back("< dash left # ");
            break;
        case Control::DASH_RIGHT:
            buttonNames.emplace_back("> dash right # ");
            break;
        case Control::NONE:
            LOG_ERROR("Tried to set Control::NONE!");
            break;
        default:
            LOG_ERROR("Unknown Control: %i", int(c));
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

    bodyHeight = 54 * buttons.size() + 22;
    windowBody.setScale(384, bodyHeight);
    windowBottom.setPosition(38, 216 + bodyHeight);
}

void GUIControlsOverlay::roll()
{
    LOG("Retracting!");
    bodyHeight = 0;
}

void GUIControlsOverlay::unroll()
{
    LOG("Expanding!");
    bodyHeight = 54 * buttons.size() + 22;
}
