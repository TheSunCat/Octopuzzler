#include "UIButton.h"

#include <utility>

#include "Outrospection.h"

UIButton::UIButton(const std::string& _texName, const float posXPercent, const float posYPercent, const float widthPercent,
                   const float heightPercent, std::function<void()> clickCallback) : UIComponent(_texName, posXPercent, posYPercent, widthPercent, heightPercent), onClick(
                                                                                         std::move(clickCallback))
{
    
}

bool UIButton::isOnButton(const glm::vec2& point) const
{
    return (point.x > position.x && point.x < position.x + width) && (point.y > position.y && point.y < position.y + height);
}

void UIButton::tick()
{
    glm::vec2 mousePos = Outrospection::get().lastMousePos;
    hovered = isOnButton(mousePos);

    name = std::to_string(hovered);
}
