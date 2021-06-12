#include "GUIOctopusOverlay.h"
#include "Outrospection.h"

void eyeClick(UIButton& button, float mouseX, float mouseY)
{
    LOG("<Octopus> ouch. you pressed eye %s", button.name);
}

GUIOctopusOverlay::GUIOctopusOverlay() : GUILayer("Octopus Overlay", true),
theOverlay("octopusOverlay", 0, 0, 1, 1)
{
    buttons.emplace_back(std::make_unique<UIButton>("eyeCircle",
        Outrospection::get().textureManager.loadAnimatedTexture({ "UI/eyes/", "eyeCircle" }, 3, 5),
        0, 0, 1, 1, Bounds(BoundsShape::Circle, { 0.73, 0.1, 0.09 }),
        eyeClick));
    buttons.emplace_back(std::make_unique<UIButton>("eyeSquare",
        Outrospection::get().textureManager.loadAnimatedTexture({ "UI/eyes/", "eyeSquare" },3, 5),
        0, 0, 1, 1, Bounds(BoundsShape::Circle, { 0.88, 0.05, 0.09 }),
        eyeClick));
    buttons.emplace_back(std::make_unique<UIButton>("eyeTriangle",
        Outrospection::get().textureManager.loadAnimatedTexture({ "UI/eyes/", "eyeTriangle" }, 3, 5),
        0, 0, 1, 1, Bounds(BoundsShape::Circle, { 0.93, 0.3, 0.09 }),
        eyeClick));

    captureMouse = false;
}

void GUIOctopusOverlay::tick()
{
    theOverlay.tick();

	for(auto& button: buttons)
	{
        button->tick();
	}
}

void GUIOctopusOverlay::draw() const
{
    Util::glError();
	
    theOverlay.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

    for (auto& button : buttons)
    {
        button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
	
    Util::glError();
}
