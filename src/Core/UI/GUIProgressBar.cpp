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

    leftInk.setPosition(0, 270 * curProgress - 760);
    middleInk.setPosition(0, 300 * curProgress - 760);
    rightInk.setPosition(0, 200 * curProgress - 760);
}

void GUIProgressBar::draw() const
{
    leftInk.draw(Outrospection::get().inkShader, Outrospection::get().glyphShader);
    middleInk.draw(Outrospection::get().inkShader, Outrospection::get().glyphShader);
    rightInk.draw(Outrospection::get().inkShader, Outrospection::get().glyphShader);
}

void GUIProgressBar::setProgress(float _progress)
{
    progress = _progress;
}
