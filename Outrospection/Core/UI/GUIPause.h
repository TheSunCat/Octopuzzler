#pragma once
#include "GUILayer.h"
#include "UIButton.h"

class GUIPause :
    public GUILayer
{
public:
    GUIPause();

    void tick() override;

    void draw() const override;
private:
    // TODO add this back UIButton buttonResume;
};
