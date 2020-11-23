#pragma once
#include "Outrospection.h"
#include "UIButton.h"
#include "Item/ItemStack.h"

class UIItemSlot : public UIButton
{
public:
    UIItemSlot(glm::vec2 positionPercent, glm::vec2 scalePercent);

    void updateItem(ItemStack& newItemStack);

private:
    ItemStack& itemStack = Outrospection::noItem;
};
