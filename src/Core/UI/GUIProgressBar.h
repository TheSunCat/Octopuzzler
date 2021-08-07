#pragma once
#include "GUILayer.h"
#include "UIButton.h"

class GUIProgressBar :
    public GUILayer
{
public:
    GUIProgressBar();

    void tick() override;

    void draw() const override;

    // 0 to 1
    void setProgress(float _progress);

    float progress = 0;
private:
    float curProgress = 0; // animates towards progress

    UIComponent leftInk;
    UIComponent middleInk;
    UIComponent rightInk;
};
