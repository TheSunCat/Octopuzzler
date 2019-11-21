#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Util.h"
#include "KeyBindings.h"

class PlayerController {
public:
	void updatePlayer(Player playerIn);

	void moveForward(Player playerIn);

	void moveBackward(Player playerIn);
};