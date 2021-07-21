#include "GUIWinOverlay.h"

#include "Outrospection.h"
#include "UIButton.h"

GUIWinOverlay::GUIWinOverlay() : GUILayer("Win", false),
                                 window("winWindow", GL_LINEAR, UITransform(480, 162, 480, 480))
{
    buttons.emplace_back(std::make_unique<UIButton>("closeButton", TextureManager::None, UITransform(1344, 162, 96, 65), Bounds(),
    [&] (UIButton&, int) -> void {
        Outrospection::get().stop();
    }));

}

void GUIWinOverlay::tick()
{
    window.tick();

    for (auto& button : buttons)
    {
        button->tick();
    }
}

void GUIWinOverlay::draw() const
{
    window.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

    for (auto& button : buttons)
    {
        button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
}
