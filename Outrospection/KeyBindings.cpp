#include "KeyBindings.h"

void updateKeys(GLFWwindow* window)
{
	forward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);

	backward = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);

	left = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);

	right = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
}