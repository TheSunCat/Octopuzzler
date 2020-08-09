#pragma once

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "KeyBinding.h"

class GameSettings
{
public:
    KeyBinding keyBindForward = KeyBinding(GLFW_KEY_W);
    KeyBinding keyBindBackward = KeyBinding(GLFW_KEY_S);
    KeyBinding keyBindRight = KeyBinding(GLFW_KEY_D);
    KeyBinding keyBindLeft = KeyBinding(GLFW_KEY_A);

    KeyBinding keyBindJump = KeyBinding(GLFW_KEY_SPACE);
    KeyBinding keyBindTalk = KeyBinding(GLFW_KEY_LEFT_SHIFT);
    KeyBinding keyBindExit = KeyBinding(GLFW_KEY_ESCAPE);

    KeyBinding keyBindBreak = KeyBinding(GLFW_KEY_Q);
};
