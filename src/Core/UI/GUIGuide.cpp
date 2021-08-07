#include "GUIGuide.h"

#include "Bounds.h"
#include "Outrospection.h"
#include "UIButton.h"

GUIGuide::GUIGuide() : GUILayer("Guides", false),
                           guideLeft("guideLeft", TextureManager::None, UITransform(43, 580, 400, 320)),
                           guideRight("guideRight", TextureManager::None, UITransform(1485, 520, 400, 320))
{
    if(!Outrospection::get().isSpeedrun())
    {
        guideLeft.addAnimation("bind", animatedTexture({"UI/guide/", "bind"}, 4, 29, GL_LINEAR));
        guideLeft.addAnimation("multibind", animatedTexture({"UI/guide/", "multibind"}, 4, 34, GL_LINEAR));
        guideLeft.addAnimation("moving", animatedTexture({"UI/guide/", "moving"}, 4, 18, GL_LINEAR));

        guideRight.addAnimation("bind", Outrospection::get().textureManager.get({"UI/guide/", "bind"}));
        guideRight.addAnimation("multibind", Outrospection::get().textureManager.get({"UI/guide/", "multibind"}));
        guideRight.addAnimation("moving", Outrospection::get().textureManager.get({"UI/guide/", "moving"}));
    }
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