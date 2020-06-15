#include "UIButton.h"

#include "Source.h"

UIButton::UIButton(const std::string& _texName, const float posXPercent, const float posYPercent, const float widthPercent,
                   const float heightPercent) : UIComponent(_texName, posXPercent, posYPercent, widthPercent, heightPercent)
{
	
}

void UIButton::tick()
{
	glm::vec2 mousePos = getOutrospection()->lastMousePos;
	
	hovered = (mousePos.x > this->position.x && mousePos.x < this->position.x + this->width) && (mousePos.y > this->position.y && mousePos.y < this->position.y + this->height);

	name = std::to_string(hovered);
}
