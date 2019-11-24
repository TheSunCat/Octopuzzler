#pragma once

#include "Scene.h"
#include "Player.h"
#include "PlayerController.h"
#include "Camera.h"

class Outrospection {
public:
	Outrospection();

	void run();

	Scene scene;

	Player player;

	PlayerController playerController;
private:
	void runGameLoop();

	bool glError();

	// set to false when the game loop shouldn't run
	volatile bool running = false;

	// timing
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	GLFWwindow* gameWindow;
	Shader objectShader;
	Shader screenShader;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int quadVAO;

	const unsigned int SCR_WIDTH = 1152;
	const unsigned int SCR_HEIGHT = 648;

	// camera stuff
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = SCR_HEIGHT / 2, lastY = SCR_WIDTH / 2;
	bool firstMouse = true;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void registerCallbacks();
};