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

    void setControls(const std::string& controlsStr);

    void roll();
    void unroll();

    DISALLOW_COPY_AND_ASSIGN(GUIControlsOverlay)
private:
    UIComponent windowTitle;
    UIComponent windowBody;
    UIComponent windowBottom;

    int currentBodyHeight = 0;

    int bodyHeight = 0;
};
