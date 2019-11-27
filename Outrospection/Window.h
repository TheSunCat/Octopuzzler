#pragma once
#include <GLAD\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

class Window {
public:
	Window() = default;

	GLFWwindow* window;

	Window(int _width, int _height)
	{
		WIDTH = _width;
		HEIGHT = _height;

		
	}

private:
	int WIDTH, HEIGHT;
};