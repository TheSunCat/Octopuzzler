#pragma once
#include <GLFW\glfw3.h>

extern bool keyForward;
extern bool keyBackward;
extern bool keyLeft;
extern bool keyRight;

void updateKeys(GLFWwindow* window);