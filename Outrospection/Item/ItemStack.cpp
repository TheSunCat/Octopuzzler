#include "ItemStack.h"

#include "Core.h"
#include "Outrospection.h"

ItemStack::ItemStack(ItemID _item) : itemID(_item), item(GET_ITEM(itemID))
{

}

void ItemStack::setCount(unsigned newCount)
{
    count = newCount;
}

void ItemStack::changeCount(int deltaCount)
{
    count += deltaCount;
}

void ItemStack::setItem(ItemID newItemID)
{
    itemID = newItemID;

    item = GET_ITEM(itemID);
}

bool ItemStack::isEntropy() const
{
    return itemID == 0;
}

bool ItemStack::isSameItem(const ItemStack& other) const
{
    return itemID == other.getItemID();
}

ItemID ItemStack::getItemID() const
{
    return itemID;
}

std::string& ItemStack::getName() const
{
    return item->name;
}

unsigned ItemStack::getCount() const
{
    return count;
}