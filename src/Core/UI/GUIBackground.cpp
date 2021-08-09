#include "GUIBackground.h"
#include "Outrospection.h"
#include "Core/UI/GUIScene.h"

GUIBackground::GUIBackground() : GUILayer("Background", false),
                                         background("overlay/background", GL_NEAREST, UITransform(0, 0, 1920, 1080))
{

}
void GUIBackground::tick()
{
    background.tick();
}

void GUIBackground::draw() const
{
    background.draw();
}
