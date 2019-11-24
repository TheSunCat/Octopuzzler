#include "KeyBindings.h"

bool keyForward;
bool keyBackward;
bool keyLeft;
bool keyRight;
bool keyExit;

void updateKeys(GLFWwindow* window)
{
	keyForward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);

	keyBackward = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);

	keyLeft = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);

	keyRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);

	keyExit = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
}