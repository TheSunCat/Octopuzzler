#pragma once
#include "GUILayer.h"
#include "Core/UI/UIComponent.h"

class GUIControlsOverlay :
    public GUILayer
{
public:
    GUIControlsOverlay();

    void tick() override;

    void draw() const override;

    DISALLOW_COPY_AND_ASSIGN(GUIControlsOverlay)
private:
    UIComponent window;
};
