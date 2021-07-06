#pragma once
#include "GUILayer.h"
#include "Core/UI/UIButton.h"

class GUIOctopusOverlay :
    public GUILayer
{
public:
    GUIOctopusOverlay();

    void tick() override;

    void draw() const override;

    DISALLOW_COPY_AND_ASSIGN(GUIOctopusOverlay)
private:
    UIComponent background;
    UIComponent octopus;
};
