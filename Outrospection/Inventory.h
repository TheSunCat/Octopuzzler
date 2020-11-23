#pragma once

#include "Item/ItemStack.h"

class Inventory
{
public:
    Inventory(unsigned int size = 27);

    ItemStack& getItemAtIndex(unsigned int index);

    void addItem(ItemStack itemStack, unsigned int index = 999999);


    unsigned int size() const;
private:
    std::vector<ItemStack> theInventoryItself;
};
