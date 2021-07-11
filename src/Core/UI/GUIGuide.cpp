#include "GUIGuide.h"

#include "Bounds.h"
#include "Outrospection.h"
#include "UIButton.h"

GUIGuide::GUIGuide() : GUILayer("Guides", false),
                           guideLeft("guideLeft", GL_LINEAR, 0.0355, 0.6, 0.25, 0.375),
                           guideRight("guideRight", GL_LINEAR, 0.735, 0.475, 0.25, 0.375)
{
    guideLeft.addAnimation("default", TextureManager::None);
    guideLeft.addAnimation("bind", animatedTexture({"UI/tutorial/", "bind"}, 10, 29, GL_LINEAR));
    guideLeft.addAnimation("multibind", animatedTexture({"UI/tutorial/", "multibind"}, 10, 34, GL_LINEAR));
    guideLeft.addAnimation("moving", animatedTexture({"UI/tutorial/", "moving"}, 10, 18, GL_LINEAR));

    guideRight.addAnimation("default", TextureManager::None);
    guideRight.addAnimation("bind", animatedTexture({"UI/tutorial/", "bind"}, 10, 29, GL_LINEAR));
    guideRight.addAnimation("multibind", animatedTexture({"UI/tutorial/", "multibind"}, 10, 34, GL_LINEAR));
    guideRight.addAnimation("moving", animatedTexture({"UI/tutorial/", "moving"}, 10, 18, GL_LINEAR));
}

void GUIGuide::tick()
{
    guideLeft.tick();
    guideRight.tick();

    for (auto& button : buttons)
    {
        button->tick();
    }
}

void GUIGuide::draw() const
{
    guideLeft.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    guideRight.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);

    for (auto& button : buttons)
    {
        button->draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
}

void GUIGuide::setRightGuide(std::string& name)
{
    guideRight.setAnimation(name);
}

void GUIGuide::setLeftGuide(std::string& name)
{
    guideLeft.setAnimation(name);
}