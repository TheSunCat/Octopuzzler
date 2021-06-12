#include "GUIOctopusOverlay.h"
#include "Outrospection.h"

void eyeClick(UIButton& button, int mouseButton)
{
    LOG("<Octopus> ouch. you poked me on my %s eye!", button.name);
	
    button.setAnimation("blink");
    Util::doLater([&button]() { button.setAnimation("default"); }, 300);

    Eye theEyeThatWasPoked;
	
	switch(button.name[8])
	{
    case 'C':
        theEyeThatWasPoked = Eye::CIRCLE;
        break;
    case 'S':
        theEyeThatWasPoked = Eye::SQUARE;
        break;
    case 'T':
        theEyeThatWasPoked = Eye::TRIANGLE;
        break;  
	}

	// TODO do something with mouse button. Left = do action, Right = set mouse Eye
    Outrospection::get().setEye(theEyeThatWasPoked);
}

GUIOctopusOverlay::GUIOctopusOverlay() : GUILayer("Octopus Overlay", false),
	theOverlay("octopusOverlay", 0, 0, 1, 1)
{
    buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeCircle0", 0, 0, 1, 1,
        Bounds(BoundsShape::Circle, { 0.73, 0.1, 0.07 }), eyeClick));
    buttons[0]->addAnimation("blink", animatedTexture({ "UI/eyes/", "eyeCircle" }, 3, 5));
	
    buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeSquare0", 0, 0, 1, 1,
        Bounds(BoundsShape::Circle, { 0.88, 0.05, 0.07 }), eyeClick));
    buttons[1]->addAnimation("blink", animatedTexture({ "UI/eyes/", "eyeSquare" }, 3, 5));
	
    buttons.emplace_back(std::make_unique<UIButton>("eyes/eyeTriangle0", 0, 0, 1, 1,
        Bounds(BoundsShape::Circle, { 0.93, 0.3, 0.07 }), eyeClick));
    buttons[2]->addAnimation("blink", animatedTexture({ "UI/eyes/", "eyeTriangle" }, 3, 5));
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
    theOverlay.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

    for (auto& button : buttons)
    {
        button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
}
