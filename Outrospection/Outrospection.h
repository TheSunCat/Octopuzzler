#pragma once

#include "Scene.h"
#include "Player.h"
#include "PlayerController.h"

class Outrospection {
public:
	Outrospection(GLFWwindow* _window);

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
};