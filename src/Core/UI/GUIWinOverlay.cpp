#include "GUIWinOverlay.h"

#include "Outrospection.h"
#include "UIButton.h"

GUIWinOverlay::GUIWinOverlay() : GUILayer("Win", false),
                                 floppy("floppy", GL_LINEAR, UITransform(0, 0, 1920, 1080)),
                                 window("winWindow", GL_LINEAR, UITransform(560, 220, 800, 640))
{
    buttons.emplace_back(std::make_unique<UIButton>("closeButton", TextureManager::None, UITransform(1264, 220, 96, 65), Bounds(),
    [&] (UIButton&, int) -> void {
        Outrospection::get().stop();
    }));

}

void GUIWinOverlay::tick()
{
    floppy.tick();
    window.tick();

    for (auto& button : buttons)
    {
        button->tick();
    }
}

void GUIWinOverlay::draw() const
{
    floppy.draw();
    window.draw();

    for (auto& button : buttons)
    {
        button->draw();
    }
}
