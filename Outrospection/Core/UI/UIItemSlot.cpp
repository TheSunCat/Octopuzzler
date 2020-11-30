#include "UIItemSlot.h"

#include "Outrospection.h"

#include <sstream>

UIItemSlot::UIItemSlot(glm::vec2 positionPercent, glm::vec2 scalePercent, ItemID id)
    : UIButton("button", positionPercent.x, positionPercent.y, scalePercent.x, scalePercent.y)
{
    if(id != 0)
    {
        itemStack.setItem(id);
    }

    updateName();
}

void UIItemSlot::updateItem(ItemStack& newItemStack)
{
    itemStack = newItemStack;

    updateName();
}

void UIItemSlot::updateName()
{
    if (itemStack.isEntropy()) // entropy has no name
        name = "";
    else {
        std::stringstream ss;
        ss << itemStack.getName() << " (" << itemStack.getCount() << ')';

        name = ss.str();
    }
}
