#include "UIItemSlot.h"

#include "Outrospection.h"

#include <sstream>

UIItemSlot::UIItemSlot(glm::vec2 positionPercent, glm::vec2 scalePercent)
    : UIButton("button", positionPercent.x, positionPercent.y, scalePercent.x, scalePercent.y)
{
    auto& itemRegistry = Outrospection::get().itemRegistry;

    Item* item = itemRegistry.get(itemStack.item);

    name = item->name;
}

void UIItemSlot::updateItem(ItemStack& newItemStack)
{
    itemStack = newItemStack;

    if (itemStack.item == 0) // entropy has no name
        name = "";
    else {
        std::stringstream ss;
        ss << GET_ITEM(itemStack.item)->name << " (" << itemStack.count << ')';

        name = ss.str();
    }
}
