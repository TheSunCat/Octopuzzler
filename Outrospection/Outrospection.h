#pragma once

#include "Scene.h"
#include "Player.h"
#include "PlayerController.h"

class Outrospection {
public:
	Outrospection();

	void run();

	Scene scene;

	Player player;

	PlayerController playerController;
private:
	void runGameLoop();

	// set to false when the game loop shouldn't run
	volatile bool running = false;

	GLFWwindow* gameWindow;
	Shader objectShader;
	Shader screenShader;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int quadVAO;

	// Set proper Viewport size when window is resized
	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(yoffset);
	}
};