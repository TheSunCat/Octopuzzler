#pragma once
#include "Item/Item.h"

class ItemStack
{
public:
    ItemStack(ItemID _item);

    unsigned int count = 0;

    ItemID item = 0;
};
