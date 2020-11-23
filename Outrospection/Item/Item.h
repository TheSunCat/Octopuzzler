#pragma once
#include <unordered_map>

typedef unsigned int ItemID;

class Item
{
public:
    Item(std::string _name);

    std::string name;
};