#include "GUIWelcome.h"

#include "Bounds.h"
#include "Outrospection.h"
#include "UIButton.h"

void close(UIButton&, int)
{
	Util::doLater([] {Outrospection::get().popOverlay(Outrospection::get().welcomeOverlay); }, 100);
}

GUIWelcome::GUIWelcome() : GUILayer("Welcome", false),
                           window("welcomeWindow", GL_LINEAR, 0.25, 0.15, 0.5, 0.65)
{
	buttons.emplace_back(std::make_unique<UIButton>("closeX",
		TextureManager::None,
		0.70, 0.15, 0.05, 0.07, Bounds(),
		close));
}

void GUIWelcome::tick()
{
	for (auto& button : buttons)
	{
		button->tick();
	}
}

void GUIWelcome::draw() const
{
	window.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
	
	for (auto& button : buttons)
	{
		button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
	}
}
