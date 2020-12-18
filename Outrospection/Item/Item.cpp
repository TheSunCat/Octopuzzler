#include "Item.h"

#include <string>
#include <utility>

#include "Outrospection.h"

Item::Item(std::string _name, unsigned int _maxStack) : name(std::move(_name)), maxStack(_maxStack), mass(1.0)
{
    Resource r("Textures/Item/", name);

    texture = &Outrospection::get().textureManager.get(r);
}
