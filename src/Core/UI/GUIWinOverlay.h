#pragma once
#include "Core/UI/GUILayer.h"
#include "Core/UI/UIComponent.h"

class GUIWinOverlay : public GUILayer
{
public:
    GUIWinOverlay();

    void tick() override;

    void draw() const override;

    DISALLOW_COPY_AND_ASSIGN(GUIWinOverlay)
private:
    UIComponent floppy;
    UIComponent window;
};
