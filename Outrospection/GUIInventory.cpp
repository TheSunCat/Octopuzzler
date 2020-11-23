#include "GUIInventory.h"

#include "Outrospection.h"
#include "Core/UI/UIButton.h"

GUIInventory::GUIInventory(Inventory& inv) : GUILayer("inventory", false), inventory(inv)
{
    for (unsigned int i = 0; i < inv.size(); i++)
    {
        int x = i % 5;
        int y = i / 5;

        UIItemSlot item(glm::vec2(float(x) / 5.0f, 0.25f + float(y) / 8.0f), glm::vec2(0.1, 0.1));

        item.updateItem(inv.getItemAtIndex(i));

        items.push_back(item);
    }
}

void GUIInventory::tick()
{
    for(unsigned int i = 0; i < inventory.size(); i++)
    {
        items[i].updateItem(inventory.getItemAtIndex(i));
    }

    for (UIItemSlot& itm : items)
    {
        itm.tick();
    }
}


void GUIInventory::draw() const
{
    for(const UIButton& itm: items)
    {
        itm.draw(Outrospection::get().spriteShader, Outrospection::get().glyphShader);
    }
}
