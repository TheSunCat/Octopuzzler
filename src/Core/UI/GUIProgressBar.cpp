#include "GUIProgressBar.h"
#include <Outrospection.h>

GUIProgressBar::GUIProgressBar() : GUILayer("Progress Bar", false),
                                   leftInk("ink/leftInk", GL_LINEAR, UITransform(0, 0, 116, 984)),
                                   middleInk("ink/middleInk", GL_LINEAR, UITransform(84, 0, 169, 1061)),
                                   rightInk("ink/rightInk", GL_LINEAR, UITransform(200, 0, 102, 937))
{

}

void GUIProgressBar::tick()
{
    leftInk.tick();
    middleInk.tick();
    rightInk.tick();

    curProgress = Util::lerp(curProgress, progress, 0.1);

    leftInk.setPosition(0, 200 * curProgress + 1.25 - 760);
    middleInk.setPosition(84, 240 * curProgress * 1.5 - 760);
    rightInk.setPosition(206, 50 * curProgress * 0.5 - 800);
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
