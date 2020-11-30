#pragma once
#include "Core.h"
#include "Item/Item.h"

class ItemStack
{
public:
    ItemStack(ItemID _item);

    void setCount(unsigned int newCount);
    void changeCount(int deltaCount);

    void setItem(ItemID newItemID);

    bool isEntropy() const;
    bool isSameItem(const ItemStack& other) const;


    ItemID getItemID() const;
    std::string& getName() const;
    unsigned int getCount() const;

    ItemStack& operator+(const ItemStack& right)
    {
        if (isSameItem(right))
        {
            changeCount(right.getCount());

            return *this;
        }

        LOG_ERROR("Tried to add two items of different types!");

        return *this;
    }

    ItemStack& operator+=(const ItemStack& right)
    {
        return *this + right;
    }
private:
    unsigned int count = 0;

    ItemID itemID = 0;
    Item* item;
};
