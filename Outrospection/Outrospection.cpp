#include "Outrospection.h"

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

void Outrospection::runGameLoop() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;


	// process keys
	updateKeys(gameWindow);

	// exit game on next loop iteration
	if (keyExit)
		running = false;

	// player always "faces" forward, so W goes away from camera
	player.playerRotation.y = camera.Yaw;

	playerController.acceleratePlayer(&player);
	playerController.collidePlayer(&player, scene.collision, deltaTime);
	playerController.movePlayer(&player);
	//scene.updatePhysics();

	camera.Position = player.playerPosition - (vecFromYaw(camera.Yaw) * glm::vec3(4.0)) + glm::vec3(0.0, 1.0, 0.0);

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
	glError();

	// swap buffers and poll IO events
	// -------------------------------
	glfwSwapBuffers(gameWindow);
	glfwPollEvents();
}

bool Outrospection::glError()
{
	bool ret = false;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		ret = true;

		cout << err << endl;
	}

	return ret;
}

void Outrospection::registerCallbacks()
{
	// Register OpenGL events
	glfwSetFramebufferSizeCallback(gameWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(gameWindow, mouse_callback);
	glfwSetScrollCallback(gameWindow, scroll_callback);
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