#include "GUIOctopusOverlay.h"
#include "Outrospection.h"
#include "Core/UI/GUIScene.h"

// ugh
Eye getEye(char c)
{
    Eye theEye = Eye::NONE;
    switch (c)
    {
    case 'C':
        theEye = Eye::CIRCLE;
        break;
    case 'S':
        theEye = Eye::SQUARE;
        break;
    case 'T':
        theEye = Eye::TRIANGLE;
        break;
    }

    return theEye;
}

void eyeClick(UIButton& eyeButton, int mouseButton)
{
    // play a random Eye_Poke sound
    int index = rand() / (RAND_MAX / 3);
    Outrospection::get().audioManager.play("Eye_Poke_" + std::to_string(index));

    eyeButton.setAnimation("blink");
    Util::doLater([&eyeButton]() { eyeButton.setAnimation("default"); }, 100);

    // ugh
    Eye pokedEye = getEye(eyeButton.text[8]);
    
    if (mouseButton == GLFW_MOUSE_BUTTON_RIGHT)
    {
        Outrospection::get().setEye(pokedEye);
    }
    else // do the action
    {
        ((GUIScene*)Outrospection::get().scene)->doControl(pokedEye);
    }
}

void eyeHover(UIButton& eyeButton, int)
{
    auto scene = (GUIScene*)Outrospection::get().scene;

    auto pokedEye = getEye(eyeButton.text[8]);
    scene->ghostInputQueue.clear();
    scene->ghostSprite.visible = true;
    scene->doGhostControl(pokedEye);
}

void eyeUnhover(UIButton& eyeButton, int)
{
    auto scene = (GUIScene*)Outrospection::get().scene;

    scene->ghostInputQueue.clear();
    scene->ghostSprite.visible = false;
}

void reset(UIButton&, int)
{
    Util::doLater([]
    {
        LOG_INFO("Resetting...");
        auto& o = Outrospection::get();
        ((GUIScene*)o.scene)->reset();
    }, 100);
}

void undo(UIButton&, int)
{
    Util::doLater([]
    {
        auto& o = Outrospection::get();
        ((GUIScene*)o.scene)->tryUndo();
    }, 100);
}

void muteOrUnmute(UIButton& muteButton, int)
{
    static bool muted = false;

    if(muted) // unmute
    {
        muted = false;
        Outrospection::get().audioManager.setGlobalVolume(1.0);
        muteButton.setAnimation("mute");
    } else
    {
        muted = true;
        Outrospection::get().audioManager.setGlobalVolume(0.0);
        muteButton.setAnimation("unmute");
    }
}

GUIOctopusOverlay::GUIOctopusOverlay() : GUILayer("Octopus Overlay", false),
                                         octopus("octopus", animatedTexture({ "UI/overlay/", "octopus" },
                                             2, 21, GL_NEAREST), UITransform(0, 0, 1920, 1080))
{
    auto &bCircle = buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeCircle0", GL_NEAREST, UITransform(1270, 0, 256, 235),
                                            Bounds(UITransform(1401, 108, 134), BoundsShape::Circle), eyeClick));
    bCircle->addAnimation("blink", animatedTexture({"UI/eyes/", "eyeCircle"}, 1, 5, GL_LINEAR));
    bCircle->onHover = eyeHover;
    bCircle->onUnhover = eyeUnhover;

    auto& bSquare = buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeSquare0", GL_NEAREST, UITransform(1551, 0, 269, 166),
                                            Bounds(UITransform(1690, 40, 134), BoundsShape::Circle), eyeClick));
    bSquare->addAnimation("blink", animatedTexture({"UI/eyes/", "eyeSquare"}, 1, 5, GL_LINEAR));
    bSquare->onHover = eyeHover;
    bSquare->onUnhover = eyeUnhover;

    auto& bTriangle = buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeTriangle0", GL_NEAREST, UITransform(1648, 180, 272, 262),
                                            Bounds(UITransform(1786, 300, 134), BoundsShape::Circle), eyeClick));
    bTriangle->addAnimation("blink", animatedTexture({"UI/eyes/", "eyeTriangle"}, 1, 5, GL_LINEAR));
    bTriangle->onHover = eyeHover;
    bTriangle->onUnhover = eyeUnhover;

    // gap of 26 px between buttons
    buttons.emplace_back(std::make_unique<UIButton>("reset", GL_NEAREST, UITransform(1798, 958, 96, 96), Bounds(), reset));
    buttons.emplace_back(std::make_unique<UIButton>("undo", GL_NEAREST, UITransform(1676, 958, 96, 96), Bounds(), undo));

    auto& bMute = buttons.emplace_back(std::make_unique<UIButton>("muteButton", TextureManager::None, UITransform(1554, 958, 96, 96), Bounds(), muteOrUnmute));
    bMute->addAnimation("unmute", simpleTexture({"UI/", "unmute"}, GL_NEAREST));
    bMute->addAnimation("mute", simpleTexture({"UI/", "mute"}, GL_NEAREST));
    bMute->setAnimation("mute");
}
void GUIOctopusOverlay::tick()
{
    octopus.tick();

    for (auto& button : buttons)
    {
        button->tick();
    }
}

void GUIOctopusOverlay::draw() const
{
    octopus.draw();

    for (auto& button : buttons)
    {
        button->draw();
    }
}
