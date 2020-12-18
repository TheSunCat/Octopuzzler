#pragma once

#include "Core/Rendering/SimpleTexture.h"

typedef unsigned int ItemID;

class Item
{
public:
    Item(std::string _name, unsigned int _maxStack = 16);

    std::string name;

    unsigned int maxStack;

    float mass;

private:
    SimpleTexture* texture;
};