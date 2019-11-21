#pragma once
#include <GLAD\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

class Window {
public:
	GLFWwindow* window;

	Window(int _width, int _height)
	{
		WIDTH = _width;
		HEIGHT = _height;

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // add on Mac bc Apple is big dumb :(
#endif

		// Window init
		window = glfwCreateWindow(WIDTH, HEIGHT, "Outrospection", NULL, NULL);
		if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(window);

		// Capture mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Load OGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
	}

private:
	int WIDTH, HEIGHT;
};