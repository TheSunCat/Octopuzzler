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
        buttons.emplace_back(std::make_unique<UIButton>("guideLeftClose", TextureManager::None, UITransform(408, 575, 25, 25), Bounds(), [](UIButton&, int)
        {
            ((GUIGuide*)Outrospection::get().guideOverlay)->setLeftGuide("closed");
        }));

        guideRight.addAnimation("bind", Outrospection::get().textureManager.get({"UI/guide/", "bind"}));
        guideRight.addAnimation("multibind", Outrospection::get().textureManager.get({"UI/guide/", "multibind"}));
        guideRight.addAnimation("moving", Outrospection::get().textureManager.get({"UI/guide/", "moving"}));
        buttons.push_back(std::make_unique<UIButton>("guideRightClose", TextureManager::None, UITransform(1850, 515, 40, 40), Bounds(), [](UIButton&, int)
        {
            ((GUIGuide*)Outrospection::get().guideOverlay)->setRightGuide("closed");
        }));
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
    guideLeft.draw();
    guideRight.draw();

    for (auto& button : buttons)
    {
        button->draw();
    }
}

void GUIGuide::setRightGuide(const std::string& name)
{
    if (name == "hidden")
        guideRight.visible = false;
    else
        guideRight.setAnimation(name);
}

void GUIGuide::setLeftGuide(const std::string& name)
{
    if (name == "hidden")
        guideLeft.visible = false;
    else
        guideLeft.setAnimation(name);
}