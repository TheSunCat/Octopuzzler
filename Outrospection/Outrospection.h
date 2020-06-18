#pragma once

#include "Constants.h"
#include "Controller.h"
#include "GameSettings.h"

#include "Core/Rendering/FreeType.h"
#include "Core/Rendering/OpenGL.h"
#include "Core/Rendering/Shader.h"
#include "Core/Rendering/TextureManager.h"

#include "Core/Camera.h"
#include "Core/Scene.h"
#include "Core/UI/GUIIngame.h"
#include "Core/UI/GUIPause.h"
#include "Core/UI/GUIScreen.h"
#include "Core/World/Player.h"
#include "Core/World/PlayerController.h"

class Outrospection {
	OpenGL opengl; // defined at the beginning so nothing gets initialized before this
	FreeType freetype;
	
public:
	Outrospection();

	void run();

	void pauseGame();
	void unpauseGame();

	void setGUIScreen(GUIScreen* screen, bool replace = true);

	void captureMouse(bool doCapture) const;

	glm::vec2 lastMousePos = glm::vec2(SCR_HEIGHT / 2.0f, SCR_WIDTH / 2.0f);
	
	Scene scene;
	Player player;
	PlayerController playerController;
	TextureManager textureManager;
	GameSettings gameSettings;
	Controller controller{};

	std::unordered_map<char, FontCharacter> fontCharacters;

	DISALLOW_COPY_AND_ASSIGN(Outrospection)
private:
	void runGameLoop();
	void runTick();
	void updateCamera();

	std::unique_ptr<GUIScreen> ingameGUI = std::make_unique<GUIIngame>();
	std::unique_ptr<GUIScreen> pauseGUI = std::make_unique<GUIPause>();

	// set to false when the game loop shouldn't run
	volatile bool running = false;

	// timing
	float deltaTime = 0.0f;	// Time between current frame and last frame
	double lastFrame = 0.0f; // Time of last frame

	GLFWwindow* gameWindow;
	
	Shader objectShader;
	Shader billboardShader;
	Shader skyShader;
	Shader screenShader;
	Shader simpleShader;
	Shader spriteShader;
	Shader glyphShader;

	GLuint framebuffer, intermediateFBO = 0;
	GLuint textureColorbuffer;
	GLuint quadVAO;

	// camera stuff
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	bool firstMouse = true;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xPosD, double yPosD);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void error_callback(int errorcode, const char* description);

	void registerCallbacks() const;
	void createShaders();
	void updateInput();

	bool isGamePaused{};

	std::vector<GUIScreen*> loadedGUIs;
};
