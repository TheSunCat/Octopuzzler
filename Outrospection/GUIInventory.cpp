#include "GUIInventory.h"

#include "Outrospection.h"
#include "Core/UI/UIButton.h"

GUIInventory::GUIInventory() : GUILayer("inventory", false)
{
    for (int x = 0; x < 5; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            UIButton item("button", float(x) / 5.0f, 0.5f + float(y) / 8.0f, 0.1, 0.1);
            items.push_back(item);
        }
    }
}

void GUIInventory::tick()
{
    for (UIButton& itm : items)
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
