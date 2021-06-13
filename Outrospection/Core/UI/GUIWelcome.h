#pragma once
#include "Core/UI/GUILayer.h"
#include "Core/UI/UIComponent.h"

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
