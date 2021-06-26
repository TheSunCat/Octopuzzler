#include "LayerStack.h"

#include <algorithm>

#include "Layer.h"

LayerStack::~LayerStack()
{
    for (Layer* layer : layers)
    {
        layer->onDetach();
        delete layer;
    }
}

void LayerStack::pushLayer(Layer* layer)
{
    layers.emplace(layers.begin() + layerInsertIndex, layer);
    layerInsertIndex++;
}

void LayerStack::pushOverlay(Layer* overlay)
{
    layers.emplace_back(overlay);
}

void LayerStack::popLayer(Layer* layer)
{
    auto it = std::find(layers.begin(), layers.begin() + layerInsertIndex, layer);
    if (it != layers.begin() + layerInsertIndex)
    {
        layers.erase(it);
        layerInsertIndex--;
    }
}

void LayerStack::popOverlay(Layer* overlay)
{
    auto it = std::find(layers.begin() + layerInsertIndex, layers.end(), overlay);
    if (it != layers.end())
    {
        layers.erase(it);
    }
}
