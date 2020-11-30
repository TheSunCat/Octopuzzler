#pragma once
#include <unordered_map>

typedef unsigned int ItemID;

class Item
{
public:
    Item(std::string _name, unsigned int _maxStack = 16);

    std::string name;

    unsigned int maxStack;

    float mass;
};