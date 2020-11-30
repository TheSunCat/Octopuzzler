#include "Item.h"

#include <string>
#include <utility>

Item::Item(std::string _name, unsigned int _maxStack) : name(std::move(_name)), maxStack(_maxStack), mass(1.0)
{
    

}