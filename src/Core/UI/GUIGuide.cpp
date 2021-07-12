#include "GUIGuide.h"

#include "Bounds.h"
#include "Outrospection.h"
#include "UIButton.h"

GUIGuide::GUIGuide() : GUILayer("Guides", false),
                           guideLeft("guideLeft", TextureManager::None, UITransform(69, 458, 405, 405)),
                           guideRight("guideRight", TextureManager::None, UITransform(1411, 513, 405, 405))
{
    guideLeft.addAnimation("bind", animatedTexture({"UI/guide/", "bind"}, 5, 29, GL_LINEAR));
    guideLeft.addAnimation("multibind", animatedTexture({"UI/guide/", "multibind"}, 5, 34, GL_LINEAR));
    guideLeft.addAnimation("moving", animatedTexture({"UI/guide/", "moving"}, 5, 18, GL_LINEAR));

    guideRight.addAnimation("bind", animatedTexture({"UI/guide/", "bind"}, 5, 29, GL_LINEAR));
    guideRight.addAnimation("multibind", animatedTexture({"UI/guide/", "multibind"}, 5, 34, GL_LINEAR));
    guideRight.addAnimation("moving", animatedTexture({"UI/guide/", "moving"}, 5, 18, GL_LINEAR));
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