#pragma once
#include "KeyBinding.h"
#include <GLFW\glfw3.h>
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