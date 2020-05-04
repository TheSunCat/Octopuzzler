#pragma once

#include <GLAD/glad.h>
#include <glfw/glfw3.h>

#include "KeyBinding.h"

class GameSettings
{
public:
	KeyBinding keyBindForward = KeyBinding(GLFW_KEY_W);
	KeyBinding keyBindBackward = KeyBinding(GLFW_KEY_S);
	KeyBinding keyBindRight = KeyBinding(GLFW_KEY_D);
	KeyBinding keyBindLeft = KeyBinding(GLFW_KEY_A);

	KeyBinding keyBindJump = KeyBinding(GLFW_KEY_SPACE);
	KeyBinding keyBindExit = KeyBinding(GLFW_KEY_ESCAPE);
};