#include "GUIPause.h"
#include <Outrospection.h>

GUIPause::GUIPause() : GUILayer("Pause Menu", false), buttonResume("button", .45f, .45f, .1f, .1f)
{
    
}

void GUIPause::tick()
{
    buttonResume.tick();
}

void GUIPause::draw() const
{
    buttonResume.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
}