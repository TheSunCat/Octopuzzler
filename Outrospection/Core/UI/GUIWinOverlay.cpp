#include "GUIWinOverlay.h"

#include "Outrospection.h"
#include "UIButton.h"

GUIWinOverlay::GUIWinOverlay() : GUILayer("Win", false),
window("winWindow", 0.25, 0.15, 0.5, 0.65)
{
	
}

void GUIWinOverlay::tick()
{
	
}

void GUIWinOverlay::draw() const
{
	window.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
}