#pragma once
#include "Core/UI/GUILayer.h"
#include "Core/UI/UIItemSlot.h"

class Inventory;
class UIButton;

class GUIInventory :
    public GUILayer
{
public:
    GUIInventory(Inventory& inv);

    void tick() override;

    void draw() const override;

private:
    std::vector<UIItemSlot> items;

    Inventory& inventory;
};

