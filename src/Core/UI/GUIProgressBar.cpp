#include "GUIProgressBar.h"
#include <Outrospection.h>

GUIProgressBar::GUIProgressBar() : GUILayer("Progress Bar", false),
                                   leftInk("ink/leftInk", GL_LINEAR, UITransform(0, 0, 1920, 1080)),
                                   middleInk("ink/middleInk", GL_LINEAR, UITransform(0, 0, 1920, 1080)),
                                   rightInk("ink/rightInk", GL_LINEAR, UITransform(0, 0, 1920, 1080))
{

}

void GUIProgressBar::tick()
{
    leftInk.tick();
    middleInk.tick();
    rightInk.tick();

    curProgress = Util::lerp(curProgress, progress, 0.1);

    leftInk.setPosition(0, 1080.f * curProgress - 800);
    middleInk.setPosition(0, 1080.f * curProgress - 800);
    rightInk.setPosition(0, 1080.f * curProgress - 800);
}

void GUIProgressBar::draw() const
{
    leftInk.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    middleInk.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    rightInk.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
}

void GUIProgressBar::setProgress(float _progress)
{
    progress = _progress;
}
