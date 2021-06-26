#pragma once
#include "GUILayer.h"
#include "UIComponent.h"

class GUIWelcome : public GUILayer
{
public:
    GUIWelcome();

    void tick() override;

    void draw() const override;

    DISALLOW_COPY_AND_ASSIGN(GUIWelcome)
private:
    UIComponent window;
};
