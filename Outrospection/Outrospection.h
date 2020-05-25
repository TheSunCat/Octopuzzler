#pragma once

#include "GameSettings.h"
#include "Controller.h"
#include "Constants.h"

#include "Core/Rendering/TextureManager.h"
#include "Core/Rendering/OpenGL.h"
#include "Core/Rendering/Shader.h"

#include "Core/PlayerController.h"
#include "Core/Scene.h"
#include "Core/Player.h"
#include "Core/Camera.h"
#include "Core/UI/GUIScreen.h"

class Outrospection {
	OpenGL opengl; // defined at the beginning so nothing gets initialized before this
	
public:
	Outrospection();

	void run();

	void pauseGame();
	void unpauseGame();

	void setGUIScreen(GUIScreen& screen, const bool replace = true);
	
	Scene scene;
	Player player;
	PlayerController playerController;
	TextureManager textureManager;
	GameSettings gameSettings;
	Controller controller{};

	DISALLOW_COPY_AND_ASSIGN(Outrospection)
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
	Shader spriteShader;

	GLuint framebuffer, intermediateFBO = 0;
	GLuint textureColorbuffer;
	GLuint quadVAO;

	// camera stuff
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = SCR_HEIGHT / 2.0f, lastY = SCR_WIDTH / 2.0f;
	bool firstMouse = true;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xPosD, double yPosD);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void registerCallbacks() const;
	void createShaders();
	void updateInput();

	bool isGamePaused{};

	std::vector<GUIScreen> loadedGUIs;
};
