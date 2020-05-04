#include "Outrospection.h"

#include "Util.h"
#include "Source.h"

Outrospection::Outrospection() : opengl() // init ogl
{
	gameWindow = opengl.gameWindow;
	quadVAO = opengl.quadVAO;
	framebuffer = opengl.framebuffer;
	textureColorbuffer = opengl.textureColorbuffer;

	registerCallbacks();
	createShaders();

	scene = Scene("TestLevel000");
	player = Player(glm::vec3(0.0, 3.0, 0.0), glm::vec3(0.0f));
}

void Outrospection::run() {
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
}

void Outrospection::unpauseGame()
{
	isGamePaused = false;
}

void Outrospection::runGameLoop() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	updateInput();
	// exit game on next loop iteration
	if (controller.pause)
		running = false;

	// player always "faces" forward, so W goes away from camera
	player.playerRotation.y = camera.Yaw;


	if (!isGamePaused) {
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
	objectShader.setVec3("viewPos", camera.Position);
	objectShader.setFloat("shininess", 32.0f);
	objectShader.setVec3("lightPos", camera.Position);
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

	Util::glError(true);

	// Bind to default framebuffer and draw ours
	// -----------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	screenShader.use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);

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
	playerController.acceleratePlayer(&player, controller);
	playerController.collidePlayer(&player, scene.collision, deltaTime);
	playerController.animatePlayer(&player);
	playerController.movePlayer(&player, deltaTime);

	updateCamera();
}

void Outrospection::updateCamera()
{
	// Calculate camera position w/ collision
	glm::vec3 playerHeadPos = player.playerPosition + glm::vec3(0.0, 0.7, 0.0);

	glm::vec3 cameraCastDir = glm::normalize(-Util::vecFromYaw(camera.Yaw));
	Ray cameraRay = Ray{playerHeadPos, cameraCastDir };

	RayHit closestHit = RayHit{ INFINITY };
	for (Triangle& t : scene.collision) {
		// invert normal bc we're looking backwards
		t.n = -t.n;

		RayHit hit = Util::rayCast(cameraRay, t, false);
		if (hit.dist < closestHit.dist)
			closestHit = hit;

		// reset normal so it doesn't affect our collision
		t.n = -t.n;
	}

	closestHit.dist -= 0.2;

	glm::vec3 target;

	if (closestHit.dist != INFINITY && closestHit.dist < 4.0f) {
		target = playerHeadPos + cameraCastDir * closestHit.dist - (closestHit.tri.n * 0.1f);
	}
	else {
		target = playerHeadPos + cameraCastDir * 4.0f;
	}

	// lerp
	camera.Position = target;// glm::mix(camera.Position, target, .25);
}

void Outrospection::registerCallbacks()
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
}

// Set proper Viewport size when window is resized
void Outrospection::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Outrospection::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	Outrospection* orig = (Outrospection*)glfwGetWindowUserPointer(window);

	if (orig->firstMouse) {
		orig->lastX = xpos;
		orig->lastY = ypos;
		orig->firstMouse = false;
	}

	float xoffset = xpos - orig->lastX;
	float yoffset = orig->lastY - ypos;

	orig->lastX = xpos;
	orig->lastY = ypos;

	orig->camera.ProcessMouseMovement(xoffset, yoffset);
}

void Outrospection::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Outrospection* orig = (Outrospection*)glfwGetWindowUserPointer(window);
	orig->camera.ProcessMouseScroll(yoffset);
}

void Outrospection::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const GameSettings& gameSettings = getOutrospection()->gameSettings;
	Controller& controller = getOutrospection()->controller;

	if (GLFW_RELEASE)
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
	}
}

void Outrospection::updateInput()
{
	unsigned int joystick = -1;
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++)
	{
		if (glfwJoystickPresent(i))
		{
			joystick = i;

			if (DEBUG)
				std::cout << "Joystick " << glfwGetJoystickName(joystick)
					<< " detected with ID " << joystick << "!" << std::endl;

			break;
		}
	}

	if (joystick != -1) // there is a controller
	{
		if (glfwJoystickIsGamepad(joystick)) // easy!
		{
			GLFWgamepadstate gamepadState;
			glfwGetGamepadState(joystick, &gamepadState);


			float leftForwardAxis = gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
			Util::valFromJoystickAxis(&leftForwardAxis); // TODO make this not a pointer func lol
			controller.leftForward = leftForwardAxis;

			float leftSideAxis = gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
			Util::valFromJoystickAxis(&leftSideAxis);
			controller.leftSide = leftSideAxis;

			controller.jump = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_A];
			controller.talk = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_X];
			controller.pause = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_START];
		}
		else // have to manually set everything :c
		{
			int axesCount = -1;

			const float* rawAxes = glfwGetJoystickAxes(joystick, &axesCount);

			if (axesCount < 2) // no sticks, return for now?
			{
				joystick = -1;
			}
			else if (axesCount == 2) // one stick! assumed to be left
			{
				float forwardAxis = rawAxes[STICK_LEFT_UP];
				Util::valFromJoystickAxis(&forwardAxis);
				controller.leftForward = forwardAxis;

				float sideAxis = rawAxes[STICK_LEFT_SIDE];
				Util::valFromJoystickAxis(&sideAxis);
				controller.leftSide = sideAxis;

			}
			else if (axesCount >= 4) // two sticks or more
			{
				float forwardAxis = rawAxes[STICK_LEFT_UP];
				Util::valFromJoystickAxis(&forwardAxis);
				controller.rightForward = forwardAxis;

				float rightAxis = rawAxes[STICK_LEFT_SIDE];
				Util::valFromJoystickAxis(&rightAxis);
				controller.rightSide = rightAxis;

				float forwardAxis = rawAxes[STICK_LEFT_UP];
				Util::valFromJoystickAxis(&forwardAxis);
				controller.leftForward = forwardAxis;

				float sideAxis = rawAxes[STICK_LEFT_SIDE];
				Util::valFromJoystickAxis(&sideAxis);
				controller.leftSide = sideAxis;

			}


			int buttonCount = -1;
			const unsigned char* rawButtons = glfwGetJoystickButtons(joystick, &buttonCount);

			if (buttonCount < 4) // not enough buttons
			{
				joystick = -1;
			}
			else
			{
				controller.jump = rawButtons[BUTTON_A];
				controller.talk = rawButtons[BUTTON_X];
				controller.pause = rawButtons[BUTTON_START];
				// TODO add more controls lol
			}
		}
	}
	
	if (joystick == -1) // no *usable* controllers are present
	{

		if (glfwGetKey(gameWindow, gameSettings.keyBindForward.keyCode) == GLFW_PRESS)
		{
			controller.leftForward = 1.0;
		}
		if (glfwGetKey(gameWindow, gameSettings.keyBindBackward.keyCode) == GLFW_PRESS)
		{
			controller.leftForward = -1.0;
		}
		if (glfwGetKey(gameWindow, gameSettings.keyBindRight.keyCode) == GLFW_PRESS)
		{
			controller.leftSide = 1.0;
		}
		if (glfwGetKey(gameWindow, gameSettings.keyBindLeft.keyCode) == GLFW_PRESS)
		{
			controller.leftSide = -1.0;
		}
		if (glfwGetKey(gameWindow, gameSettings.keyBindJump.keyCode) == GLFW_PRESS)
		{
			controller.jump = true;
		}
		if (glfwGetKey(gameWindow, gameSettings.keyBindExit.keyCode) == GLFW_PRESS)
		{
			controller.pause = true;
		}
	}
}