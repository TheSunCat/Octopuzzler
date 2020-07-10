#pragma once
#include <string>

#include "UIComponent.h"
#include "Core/Layer.h"

class GUILayer : public Layer
{
public:
	explicit GUILayer(std::string _name, bool _captureMouse = true);

	void onAttach() override;
private:
	std::string name;
	bool captureMouse = false;
};
