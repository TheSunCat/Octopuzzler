#pragma once

#include "Constants.h"
#include "Core/Rendering/OpenGL.h"
#include "Core/Scene.h"
#include "Core/Player.h"
#include "Core/PlayerController.h"
#include "Core/Rendering/TextureManager.h"
#include "GameSettings.h"
#include "Controller.h"

class Outrospection {
public:
	Outrospection();

	OpenGL opengl;

	void run();

	Scene scene;

	Player player;

	PlayerController playerController;

	void pauseGame();

	void unpauseGame();

	// Check for OpenGL errors and print them
	bool glError(bool print);

	TextureManager textureManager;

	GameSettings gameSettings;

	Controller controller;
private:
	void runGameLoop();

	void runTick();

	void updateCamera();

	// set to false when the game loop shouldn't run
	volatile bool running = false;

	// timing
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	GLFWwindow* gameWindow;
	Shader objectShader;
	Shader billboardShader;
	Shader skyShader;
	Shader screenShader;
	Shader simpleShader;

	unsigned int framebuffer, intermediateFBO;
	unsigned int textureColorbuffer;
	unsigned int quadVAO;

	// camera stuff
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = SCR_HEIGHT / 2, lastY = SCR_WIDTH / 2;
	bool firstMouse = true;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void registerCallbacks();
	void createShaders();
	void updateInput();

	bool isGamePaused;
};