#include "UIButton.h"

#include <utility>

#include "Outrospection.h"

UIButton::UIButton(const std::string& _texName, const GLint& texFilter, const UITransform& _transform, Bounds bounds, ButtonCallback clickCallback)
    : UIComponent(_texName, texFilter, _transform),
      onClick(std::move(clickCallback)),
      buttonBounds(bounds)
{
    // assume default bounds
    if (buttonBounds.shape == BoundsShape::None)
    {
        buttonBounds = Bounds(transform);
    }
}

UIButton::UIButton(const std::string& _name, SimpleTexture& tex, const UITransform& _transform,
                   Bounds bounds, ButtonCallback clickCallback)
    : UIComponent(_name, tex, _transform),
      onClick(std::move(clickCallback)),
      buttonBounds(bounds)
{
    // assume default bounds
    if (buttonBounds.shape == BoundsShape::None)
    {
        buttonBounds = Bounds(transform);
    }
}

bool UIButton::isOnButton(const glm::vec2& point) const
{
    return buttonBounds.contains(point);
}

void UIButton::tick()
{
    glm::vec2 mousePos = Outrospection::get().lastMousePos;

    bool lastHovered = hovered;
    hovered = isOnButton(mousePos);

    if(onHover && !lastHovered && hovered)
    {
        onHover(*this, 0);
    }

    if (onUnhover && lastHovered && !hovered)
    {
        onUnhover(*this, 0);
    }
}
