#include "Inventory.h"

#include "Core.h"
#include "Outrospection.h"

Inventory::Inventory(unsigned int size)
{
    for(unsigned int i = 0; i < size; i++)
    {
        theInventoryItself.push_back(Outrospection::get().noItem);
    }
}

ItemStack& Inventory::getItemAtIndex(unsigned int index)
{
    if(index >= theInventoryItself.size())
    {
        return Outrospection::get().noItem;
    }

    return theInventoryItself[index];
}

void Inventory::addItem(ItemStack itemStack, unsigned int index)
{
    if(index == 999999) // none specified, find first empty slot
    {
        for(unsigned int i = 0; i < theInventoryItself.size(); i++)
        {
            ItemStack& curItmStack = theInventoryItself[i];

            if(curItmStack.isEntropy() || curItmStack.isSameItem(itemStack))
            {
                index = i;
                break;
            }
        }
    }

    if (index >= theInventoryItself.size())
        index = 0;
    

    ItemStack& itemAtIndex = theInventoryItself[index];

    if(itemAtIndex.isEntropy())
    {
        theInventoryItself[index] = itemStack;
    } else if(itemAtIndex.isSameItem(itemStack))
    {
        itemAtIndex += itemStack;
    } else
    {
        LOG_ERROR("Tried to insert item (ID: %i) at index %i where an item (ID: %i) was already stored.", itemStack.getItemID(), index, itemAtIndex.getItemID());
    }
}

unsigned Inventory::size() const
{
    return theInventoryItself.size();
}
