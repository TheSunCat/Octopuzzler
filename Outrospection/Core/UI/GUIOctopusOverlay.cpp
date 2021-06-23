#include "GUIOctopusOverlay.h"
#include "Outrospection.h"
#include "Core/UI/GUIScene.h"


void eyeClick(UIButton& button, int mouseButton)
{
    LOG("<Octopus> ouch. you poked me on my %s eye!", button.name);

    button.setAnimation("blink");
    Util::doLater([&button]() { button.setAnimation("default"); }, 100);

    auto pokedEye = (Eye)button.name[8];
    
    if (mouseButton == GLFW_MOUSE_BUTTON_RIGHT)
    {
        Outrospection::get().setEye(pokedEye);
    }
    else // do the action
    {
        Outrospection::get().doControl(pokedEye);
    }
}

void showWelcome(UIButton&, int)
{
    Util::doLater([] { Outrospection::get().pushOverlay(Outrospection::get().welcomeOverlay); }, 100);
}

void reset(UIButton&, int)
{
    Util::doLater([]
    {
        LOG("resetting...");
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

GUIOctopusOverlay::GUIOctopusOverlay() : GUILayer("Octopus Overlay", false),
                                         theOverlay("octopusOverlay", GL_LINEAR, 0, 0, 1, 1)
{
    buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeCircle0", GL_LINEAR, 0, 0, 1, 1,
                                                    Bounds(BoundsShape::Circle, {0.73, 0.1, 0.07}), eyeClick));
    buttons[0]->addAnimation("blink", animatedTexture({"UI/eyes/", "eyeCircle"}, 1, 5, GL_LINEAR));

    buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeSquare0", GL_LINEAR, 0, 0, 1, 1,
                                                    Bounds(BoundsShape::Circle, {0.88, 0.05, 0.07}), eyeClick));
    buttons[1]->addAnimation("blink", animatedTexture({"UI/eyes/", "eyeSquare"}, 1, 5, GL_LINEAR));

    buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeTriangle0", GL_LINEAR, 0, 0, 1, 1,
                                                    Bounds(BoundsShape::Circle, {0.93, 0.3, 0.07}), eyeClick));
    buttons[2]->addAnimation("blink", animatedTexture({"UI/eyes/", "eyeTriangle"}, 1, 5, GL_LINEAR));

    buttons.emplace_back(std::make_unique<UIButton>("showWelcome", GL_NEAREST, 0.95, 0.92, 0.05, 0.08, Bounds(), showWelcome));
    buttons.emplace_back(std::make_unique<UIButton>("reset", GL_NEAREST, 0.90, 0.92, 0.05, 0.08, Bounds(), reset));
    buttons.emplace_back(std::make_unique<UIButton>("undo", GL_NEAREST, 0.85, 0.92, 0.05, 0.08, Bounds(), undo));
}

void GUIOctopusOverlay::tick()
{
    theOverlay.tick();

    for (auto& button : buttons)
    {
        button->tick();
    }
}

void GUIOctopusOverlay::draw() const
{
    theOverlay.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

    for (auto& button : buttons)
    {
        button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
}
