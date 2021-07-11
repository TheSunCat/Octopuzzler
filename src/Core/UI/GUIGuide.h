#pragma once
#include "GUILayer.h"
#include "UIComponent.h"

class GUIGuide : public GUILayer
{
public:
    GUIGuide();

    void tick() override;

    void draw() const override;

    void setLeftGuide(std::string& name);
    void setRightGuide(std::string& name);

    DISALLOW_COPY_AND_ASSIGN(GUIGuide)
private:
    UIComponent guideLeft;
    UIComponent guideRight;
};
