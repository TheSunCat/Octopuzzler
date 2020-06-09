#include "Outrospection.h"

#include <glm/ext/matrix_clip_space.hpp>

#include "Util.h"
#include "Source.h"
#include "Core/UI/GUIScreen.h"

Outrospection::Outrospection() : opengl(), freetype()
{
	gameWindow = opengl.gameWindow;
	quadVAO = opengl.quadVAO;
	framebuffer = opengl.framebuffer;
	textureColorbuffer = opengl.textureColorbuffer;

	registerCallbacks();
	createShaders();

	scene = Scene("TestLevel000");
	player = Player(glm::vec3(0.0, 3.0, 0.0));

	GUIScreen ingameGUI("Ingame GUI", { UIComponent("dummy", 0, 0, 0.3, 0.3) });
	
	setGUIScreen(ingameGUI);
}

void Outrospection::run()
{
	running = true;

	while (running)
	{
		runGameLoop();
	}

	glfwTerminate();
}

void Outrospection::pauseGame()
{
	isGamePaused = true;

	GUIScreen pauseGUI("Pause GUI", { UIComponent("paused", 0.5, 0.5, 0.4, 0.4) });
	setGUIScreen(pauseGUI);
}

void Outrospection::unpauseGame()
{
	isGamePaused = false;

	GUIScreen ingameGUI("Ingame GUI", { UIComponent("dummy", 0, 0, 0.3, 0.3) });
	setGUIScreen(ingameGUI);
}

void Outrospection::setGUIScreen(GUIScreen& screen, const bool replace)
{
	if(replace && !loadedGUIs.empty())
	{
		loadedGUIs.pop_back();
	}

	// move to avoid copy
	loadedGUIs.push_back(std::move(screen));
}

void Outrospection::runGameLoop()
{
	const float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	
	// fetch input into simplified controller class
	updateInput();

	if (controller.isGamepad)
	{
		camera.playerRotateCameraBy(controller.rightSide * 12.5f, controller.rightForward * 10);
	}

	// exit game on next loop iteration
	if (controller.pause == 1)
	{
		if (isGamePaused)
			unpauseGame();
		else
			pauseGame();
	}
	
	// player always "faces" forward, so W goes away from camera
	player.yaw = camera.yaw;

	
	if (!isGamePaused)
	{
		// Run one "tick" of the game physics
		runTick();
	}

	// TODO execute scheduled tasks
	// ----------------------------


	// Bind to framebuffer and draw scene to color texture
	// ---------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Set shader info
	objectShader.use();
	objectShader.setVec3("viewPos", camera.position);
	objectShader.setFloat("shininess", 32.0f);
	objectShader.setVec3("lightPos", player.position + glm::vec3(0, 1.5f, 0));
	objectShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, true);

	billboardShader.use();
	billboardShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, true);

	skyShader.use();
	skyShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, false);

	simpleShader.use();
	simpleShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, true);

	// draw stuff
	scene.draw(objectShader, billboardShader, skyShader, simpleShader);

	player.draw(billboardShader);

	//Util::glError(true);

	glDisable(GL_DEPTH_TEST); // disable depth test so stuff near camera isn't clipped
	
	// bind to default framebuffer and draw custom one over that

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	screenShader.use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);


	// draw UI
	for(const auto& screen : loadedGUIs)
	{
		screen.draw(spriteShader);
	}
	
	glEnable(GL_DEPTH_TEST); // re-enable depth testing

	// Check for errors
	// ----------------
	Util::glError(DEBUG);

	// swap buffers and poll IO events
	// -------------------------------
	glfwSwapBuffers(gameWindow);
	glfwPollEvents();
}

void Outrospection::runTick()
{
	playerController.acceleratePlayer(player, controller, deltaTime);
	playerController.collidePlayer(player, scene.collision);
	//playerController.animatePlayer(player);
	playerController.movePlayer(player);

	updateCamera();

	for(auto& screen : loadedGUIs)
	{
		screen.tick();
	}
}

void Outrospection::updateCamera()
{
	// TODO need a better way to determine whether the camera should be auto-updated or not
	camera.calculateCameraPosition(player, scene, playerController.isMoving());
}

void Outrospection::registerCallbacks() const
{
	// Register OpenGL events
	glfwSetFramebufferSizeCallback(gameWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(gameWindow, mouse_callback);
	glfwSetScrollCallback(gameWindow, scroll_callback);
	glfwSetKeyCallback(gameWindow, key_callback);
}

void Outrospection::createShaders()
{
	objectShader    = Shader("obj"      , "obj"      );
	billboardShader = Shader("billboard", "lightless");
	skyShader       = Shader("sky"      , "sky"      );
	screenShader    = Shader("screen"   , "screen"   );
	simpleShader    = Shader("simple"   , "simple"   );
	spriteShader    = Shader("sprite"   , "sprite"   );

	// set up 2d shader
	const glm::mat4 projection = glm::ortho(0.0f, float(SCR_WIDTH),
	                                        float(SCR_HEIGHT), 0.0f, -1.0f, 1.0f);
	spriteShader.use();
	spriteShader.setMat4("projection", projection);
}

// set proper viewport size when window is resized
void Outrospection::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Outrospection::mouse_callback(GLFWwindow* window, double xPosD, double yPosD)
{
	Outrospection* orig = getOutrospection();

	const auto xPos = float(xPosD);
	const auto yPos = float(yPosD);
	
	if (orig->firstMouse) {
		orig->lastX = xPos;
		orig->lastY = yPos;
		orig->firstMouse = false;
		return; // nothing to calculate because we technically didn't move the mouse
	}

	const float xOffset = xPos - orig->lastX;
	const float yOffset = orig->lastY - yPos;

	orig->lastX = xPos;
	orig->lastY = yPos;

	orig->camera.playerRotateCameraBy(xOffset, yOffset);
}

void Outrospection::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Outrospection* orig = getOutrospection();
	orig->camera.changeDistBy(float(yoffset));
}

void Outrospection::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const GameSettings& gameSettings = getOutrospection()->gameSettings;
	Controller& controller = getOutrospection()->controller;

	if (action == GLFW_RELEASE)
	{
		if (key == gameSettings.keyBindForward.keyCode || key == gameSettings.keyBindBackward.keyCode)
		{
			controller.leftForward = 0.0;
		}
		if (key == gameSettings.keyBindRight.keyCode || key == gameSettings.keyBindLeft.keyCode)
		{
			controller.leftSide = 0.0;
		}
		if (key == gameSettings.keyBindJump.keyCode)
		{
			controller.jump = false;
		}
		if (key == gameSettings.keyBindExit.keyCode)
		{
			controller.pause = false;
		}
		if(key == gameSettings.keyBindTalk.keyCode)
		{
			controller.talk = false;
		}
		if(key == gameSettings.keyBindBreak.keyCode)
		{
			controller.debugBreak = false;
		}
	}
}

void Outrospection::updateInput()
{
	int joystick = -1;
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++)
	{
		if (glfwJoystickPresent(i) == GLFW_TRUE)
		{
			joystick = i;

			controller.isGamepad = true;

			if (VERBOSE)
				std::cout << "Joystick " << glfwGetJoystickName(joystick)
					<< " detected with ID " << joystick << "! ";

			break;
		}
	}
								
	if (joystick != -1) {					// there is a controller
		
		if (glfwJoystickIsGamepad(joystick)) // easy!
		{
			if (VERBOSE)
				std::cout << "It is a gamepad! Sweet! ";

			GLFWgamepadstate gamepadState;
			glfwGetGamepadState(joystick, &gamepadState);

			// make negative bc flipped!
			controller.leftForward = -Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
			controller.leftSide = Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);

			controller.rightForward = -Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
			controller.rightSide = Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);

			controller.leftTrigger = Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);

			controller.jump  = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_A]     ? controller.jump  + 1 : false;
			controller.talk  = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_X]     ? controller.talk  + 1 : false;
			controller.pause = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_START] ? controller.pause + 1 : false;
		}
		else // have to manually set everything :c
		{
			if (VERBOSE)
				std::cout << "It is a non-mapped controller. Hrm. ";

			int axesCount = -1;

			const float* rawAxes = glfwGetJoystickAxes(joystick, &axesCount);

			if (axesCount < 2) // no sticks, return for now?
			{
				joystick = -1;
			}
			else if (axesCount == 2) // one stick! assumed to be left so we can move around
			{
				controller.leftForward = -Util::valFromJoystickAxis(rawAxes[STICK_LEFT_UP]);
				controller.leftSide = Util::valFromJoystickAxis(rawAxes[STICK_LEFT_SIDE]);

			}
			else if (axesCount >= 4) // two sticks or more
			{
				controller.rightForward = -Util::valFromJoystickAxis(rawAxes[STICK_LEFT_UP]);
				controller.rightSide = Util::valFromJoystickAxis(rawAxes[STICK_LEFT_SIDE]);

				controller.leftForward = -Util::valFromJoystickAxis(rawAxes[STICK_LEFT_UP]);
				controller.leftSide = Util::valFromJoystickAxis(rawAxes[STICK_LEFT_SIDE]);
			}


			int buttonCount = -1;
			const unsigned char* rawButtons = glfwGetJoystickButtons(joystick, &buttonCount);

			if (buttonCount < 4) // not enough buttons for us
			{
				joystick = -1;
			}
			else
			{
				controller.jump = rawButtons[BUTTON_A] ? controller.jump + 1 : false;
				controller.talk = rawButtons[BUTTON_X] ? controller.talk + 1 : false;
				controller.pause = rawButtons[BUTTON_START] ? controller.pause + 1 : false;
				// TODO add more controls lol
			}
		}
	}
	
	if (joystick == -1) // no *usable* controllers are present
	{
		if (VERBOSE)
			std::cout << "No usable controller is present. ";

		controller.isGamepad = false;

		float leftForwardInput = 0.0f;
		if (glfwGetKey(gameWindow, gameSettings.keyBindForward.keyCode) == GLFW_PRESS)
		{
			leftForwardInput += 1.0;
		}
		if (glfwGetKey(gameWindow, gameSettings.keyBindBackward.keyCode) == GLFW_PRESS)
		{
			leftForwardInput += -1.0;
		}
		controller.leftForward = leftForwardInput;


		float leftSideInput = 0.0f;
		if (glfwGetKey(gameWindow, gameSettings.keyBindRight.keyCode) == GLFW_PRESS)
		{
			leftSideInput += 1.0;
		}
		if (glfwGetKey(gameWindow, gameSettings.keyBindLeft.keyCode) == GLFW_PRESS)
		{
			leftSideInput += -1.0;
		}
		controller.leftSide = leftSideInput;


		controller.jump = glfwGetKey(gameWindow, gameSettings.keyBindJump.keyCode) == GLFW_PRESS ? controller.jump + 1 : false;
		controller.talk = glfwGetKey(gameWindow, gameSettings.keyBindTalk.keyCode) == GLFW_PRESS ? controller.talk + 1 : false;
		controller.pause = glfwGetKey(gameWindow, gameSettings.keyBindExit.keyCode) == GLFW_PRESS ? controller.pause + 1 : false;
		controller.debugBreak = glfwGetKey(gameWindow, gameSettings.keyBindBreak.keyCode) == GLFW_PRESS ? controller.debugBreak + 1 : false;
	}

	if (VERBOSE)
		std::cout << std::endl;
}