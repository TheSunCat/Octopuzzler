#pragma once
#include <functional>

#include "Bounds.h"
#include "Core/UI/UIComponent.h"

class UIButton :
    public UIComponent
{
    typedef std::function<void(UIButton&, int)> ButtonCallback;

public:
    UIButton(const std::string& _texName, const GLint& texFilter, const UITransform& transform,
             Bounds bounds, ButtonCallback clickCallback = nullptr);

    UIButton(const std::string& _name, SimpleTexture& tex, const UITransform& transform,
             Bounds bounds, ButtonCallback clickCallback = nullptr);

    bool isOnButton(const glm::vec2& point) const;

    void tick() override;

    ButtonCallback onClick;
    ButtonCallback onHover;
    ButtonCallback onUnhover;

    bool hovered = false;

    Bounds buttonBounds;
};
