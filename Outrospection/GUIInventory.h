#pragma once
#include "Core/UI/GUILayer.h"

class UIButton;

class GUIInventory :
    public GUILayer
{
public:
    GUIInventory();

    void tick() override;

    void draw() const override;

private:
    std::vector<UIButton> items;
};

