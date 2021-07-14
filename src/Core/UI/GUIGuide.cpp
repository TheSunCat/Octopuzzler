#include "GUIGuide.h"

#include "Bounds.h"
#include "Outrospection.h"
#include "UIButton.h"

GUIGuide::GUIGuide() : GUILayer("Guides", false),
                           guideLeft("guideLeft", TextureManager::None, 0.0355, 0.6, 0.25, 0.375),
                           guideRight("guideRight", TextureManager::None, 0.735, 0.475, 0.25, 0.375)
{
    guideLeft.addAnimation("bind", animatedTexture({"UI/guide/", "bind"}, 4, 29, GL_LINEAR));
    guideLeft.addAnimation("multibind", animatedTexture({"UI/guide/", "multibind"}, 4, 34, GL_LINEAR));
    guideLeft.addAnimation("moving", animatedTexture({"UI/guide/", "moving"}, 4, 18, GL_LINEAR));

    guideRight.addAnimation("bind", animatedTexture({"UI/guide/", "bind"}, 4, 29, GL_LINEAR));
    guideRight.addAnimation("multibind", animatedTexture({"UI/guide/", "multibind"}, 4, 34, GL_LINEAR));
    guideRight.addAnimation("moving", animatedTexture({"UI/guide/", "moving"}, 4, 18, GL_LINEAR));
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