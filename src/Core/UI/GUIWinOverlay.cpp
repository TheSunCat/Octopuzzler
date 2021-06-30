#include "GUIWinOverlay.h"

#include "Outrospection.h"
#include "UIButton.h"

GUIWinOverlay::GUIWinOverlay() : GUILayer("Win", false),
                                 window("winWindow", GL_LINEAR, 0.25, 0.15, 0.5, 0.65)
{
    buttons.emplace_back(std::make_unique<UIButton>("closeButton", TextureManager::None, 0.70, 0.15, 0.05, 0.06, Bounds(), [&] (UIButton&, int) -> void{
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