#pragma once

#include "Events/Event.h"

class Layer
{
public:
    Layer() = default;
    virtual ~Layer() = default;

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void tick() {}
    virtual void draw() const {}
    virtual void onEvent(Event& event) {}

    // hacky
    bool handleManually = false;
};