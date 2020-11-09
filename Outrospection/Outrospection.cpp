#include "Outrospection.h"

#include <glm/ext/matrix_clip_space.hpp>
#include "irrKlang/irrKlang.h"

#include "Util.h"
#include "Core/Layer.h"

#include "Core/UI/GUIIngame.h"
#include "Core/UI/GUILayer.h"
#include "Core/UI/GUIPause.h"
#include "Core/World/PhysicsValues.h"
#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

void* operator new(const size_t _Size)
{
    //std::cout << s << '\n';

    return malloc(_Size);
}

Outrospection* Outrospection::instance = nullptr;

irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

Outrospection::Outrospection()
{
    instance = this;

    ingameGUI = new GUIIngame();
    pauseGUI = new GUIPause();

    gameWindow = opengl.gameWindow;
    quadVAO = opengl.quadVAO;
    framebuffer = opengl.framebuffer;
    textureColorbuffer = opengl.textureColorbuffer;

    fontCharacters = freetype.loadedCharacters;

    registerCallbacks();
    createShaders();

    scene = Scene("TestLevel000");
    player = Player(glm::vec3(30.0, 0.0, 0.0));

    pushOverlay(ingameGUI);

    soundEngine->play2D("./res/ObjectData/Sounds/taco.ogg", false);
}

void Outrospection::run()
{
    running = true;

    lastFrame = currentTimeMillis;
    while (running)
    {
        runGameLoop();
    }

    glfwTerminate();
}

void Outrospection::onEvent(Event& e)
{
    dispatchEvent<WindowCloseEvent>(e, BIND_EVENT_FUNC(Outrospection::onWindowClose));
    //dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnWindowResize));
    dispatchEvent<MouseMovedEvent>(e, BIND_EVENT_FUNC(Outrospection::onMouseMoved));

    for (auto it = layerStack.rbegin(); it != layerStack.rend(); ++it)
    {
        if (e.handled)
            break;
        (*it)->onEvent(e);
    }
}

void Outrospection::pushLayer(Layer* layer)
{
    layerStack.pushLayer(layer);
    layer->onAttach();
}

void Outrospection::pushOverlay(Layer* overlay)
{
    layerStack.pushOverlay(overlay);
    overlay->onAttach();
}

void Outrospection::captureMouse(const bool doCapture) const
{
    if (doCapture)
        glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
    {
        glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetCursorPos(gameWindow, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);
    }
}

void Outrospection::runGameLoop()
{
    const auto currentFrame = currentTimeMillis;
    deltaTime = float(currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    if(deltaTime == 0.0f)
    {
        deltaTime = 1.0f / 60.0f;
    }

    //LOG_DEBUG("%f", deltaTime);
    // Update game world
    {
        // fetch input into simplified controller class
        updateInput();

        // exit game on next loop iteration
        if (controller.pause == 1)
        {
            //if (isGamePaused)
                //unpauseGame();
            //else
                //pauseGame();
        }


        if (!isGamePaused)
        {
            // Run one "tick" of the game physics
            runTick();

            // TODO execute scheduled tasks
            // ----------------------------
        }

        // UIs are also updated when game is paused
        for (auto& layer : layerStack)
        {
            layer->tick();
        }
    }

    // Draw the frame!
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // set shader info
        objectShader.use();
        objectShader.setVec3("viewPos", camera.mPosition);
        objectShader.setFloat("shininess", 32.0f);
        objectShader.setVec3("lightPos", player.position + glm::vec3(0, 1.5f, 0));
        objectShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, true);

        billboardShader.use();
        billboardShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, true);

        skyShader.use();
        skyShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, false);

        simpleShader.use();
        simpleShader.doProjView(camera, SCR_WIDTH, SCR_HEIGHT, true);

        // draw stuffs
        scene.draw(objectShader, billboardShader, skyShader, simpleShader);
        player.draw(billboardShader);


        glDisable(GL_DEPTH_TEST); // disable depth test so stuff near camera isn't clipped

        // bind to default framebuffer and draw custom one over that

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);    // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // draw UI
        for (const auto& layer : layerStack)
        {
            layer->draw();
        }

        glEnable(GL_DEPTH_TEST); // re-enable depth testing
    }

    // check for errors
    Util::glError();

    // swap buffers and poll IO events
    // -------------------------------
    glfwSwapBuffers(gameWindow);
    glfwPollEvents();
}

float largestDist = 0.0f;
void Outrospection::runTick()
{
    // update gravity direction & strength
    gravity = glm::normalize(glm::vec3(0) - player.position);
    //gravityStrength = 9.8f;


    playerController.acceleratePlayer(player, controller, glm::cross(gravity, camera.mRight), -camera.mUp, deltaTime);
    playerController.collidePlayer(player, scene.collision, deltaTime);
    playerController.animatePlayer(player);
    playerController.movePlayer(player, deltaTime);

    if (controller.isGamepad)
        camera.playerRotateCameraBy(controller.rStickX * 12.5f, controller.rStickY * 10);

    updateCamera();

    float dist = glm::distance(glm::vec3(0, 0, 0), player.position) - 20.0f;

    if(dist > largestDist)
    {
        //LOG_DEBUG("New largest distance %f", dist);

        largestDist = dist;
    }
}

void Outrospection::updateCamera()
{
    // TODO need a better way to determine whether the camera should be auto-updated or not
    camera.calculateCameraPosition(player, scene, deltaTime, playerController.isMoving());
}

void Outrospection::registerCallbacks() const
{
    // Register OpenGL events
    glfwSetFramebufferSizeCallback(gameWindow, [](GLFWwindow*, const int width, const int height)
    {
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(gameWindow, [](GLFWwindow* window, const double xPosD, const double yPosD)
    {
        MouseMovedEvent event(xPosD, yPosD);
        Outrospection::get().onEvent(event);
    });

    glfwSetMouseButtonCallback(gameWindow, [](GLFWwindow* window, const int button, const int action, const int mods)
    {
        switch (action)
        {
        case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                Outrospection::get().onEvent(event);
                break;
            }
        case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                Outrospection::get().onEvent(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(gameWindow, [](GLFWwindow* window, const double xDelta, const double yDelta)
    {
        MouseScrolledEvent event(xDelta, yDelta);
        Outrospection::get().onEvent(event);
    });

    //glfwSetKeyCallback(gameWindow, key_callback);
    glfwSetErrorCallback(error_callback);
}

void Outrospection::createShaders()
{
    objectShader    = Shader("obj"      , "obj"      );
    billboardShader = Shader("billboard", "lightless");
    skyShader       = Shader("sky"      , "sky"      );
    screenShader    = Shader("screen"   , "screen"   );
    simpleShader    = Shader("simple"   , "simple"   );
    spriteShader    = Shader("sprite"   , "sprite"   );
    glyphShader     = Shader("sprite"   , "glyph"    );

    // set up 2d shader
    const glm::mat4 projection = glm::ortho(0.0f, float(SCR_WIDTH),
                                            float(SCR_HEIGHT), 0.0f, -1.0f, 1.0f);
    spriteShader.use();
    spriteShader.setMat4("projection", projection);

    glyphShader.use();
    glyphShader.setMat4("projection", projection);
}

bool Outrospection::onWindowClose(WindowCloseEvent& e)
{
    running = false;

    return true;
}

bool Outrospection::onMouseMoved(MouseMovedEvent& e)
{
    const auto xPos = float(e.getX());
    const auto yPos = float(e.getY());

    if (firstMouse)
    {
        lastMousePos.x = xPos;
        lastMousePos.y = yPos;
        firstMouse = false;
        return true; // nothing to calculate because we technically didn't move the mouse
    }

    const float xOffset = xPos - lastMousePos.x;
    const float yOffset = lastMousePos.y - yPos;

    lastMousePos.x = xPos;
    lastMousePos.y = yPos;

    camera.playerRotateCameraBy(xOffset, yOffset);

    return false;
}

void Outrospection::scroll_callback(GLFWwindow*, const double xoffset, const double yoffset)
{
    camera.changeDistBy(float(yoffset));
}

void Outrospection::error_callback(const int errorcode, const char* description)
{
    LOG_ERROR("GLFW error (%i): %s", errorcode, description);
}

void setKey(ControllerButton& button, const int keyCode, GLFWwindow* window)
{
    button = glfwGetKey(window, keyCode) == GLFW_PRESS ? button + 1 : false;
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

            //LOG_DEBUG("Joystick %s detected with ID %i!", glfwGetJoystickName(joystick), joystick);

            break;
        }
    }
                                
    if (joystick != -1) {                    // there is a controller
        
        if (glfwJoystickIsGamepad(joystick)) // easy!
        {
            //LOG_DEBUG("It is a gamepad! Sweet!");

            GLFWgamepadstate gamepadState;
            glfwGetGamepadState(joystick, &gamepadState);

            // make negative bc flipped!
            controller.lStickY = -Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
            controller.lStickX = Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);

            controller.rStickY = -Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
            controller.rStickX = Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);

            controller.leftTrigger = Util::valFromJoystickAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);

            controller.jump  = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_A]     ? controller.jump  + 1 : false;
            controller.talk  = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_X]     ? controller.talk  + 1 : false;
            controller.pause = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_START] ? controller.pause + 1 : false;
        }
        else // have to manually set everything :c
        {
            //LOG_DEBUG("It is a non-mapped controller. Hrm.");

            int axesCount = -1;

            const float* rawAxes = glfwGetJoystickAxes(joystick, &axesCount);

            if (axesCount < 2) // not enough sticks, return for now?
            {
                joystick = -1;
            }
            else if (axesCount == 2) // one stick! assumed to be left so we can move around
            {
                controller.lStickY = -Util::valFromJoystickAxis(rawAxes[STICK_LEFT_UP]);
                controller.lStickX = Util::valFromJoystickAxis(rawAxes[STICK_LEFT_SIDE]);
            }
            else if (axesCount >= 4) // two sticks or more
            {
                controller.rStickY = -Util::valFromJoystickAxis(rawAxes[STICK_LEFT_UP]);
                controller.rStickX = Util::valFromJoystickAxis(rawAxes[STICK_LEFT_SIDE]);

                controller.lStickY = -Util::valFromJoystickAxis(rawAxes[STICK_LEFT_UP]);
                controller.lStickX = Util::valFromJoystickAxis(rawAxes[STICK_LEFT_SIDE]);
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

    // we check this for zero because a fake controller may be plugged in. real controllers will usually never have zero
    if (controller.lStickY == 0.0f || joystick == -1) // no *usable* controllers are present
    {
        //LOG_DEBUG("No usable controller is present. ");

        controller.isGamepad = false;

        float leftForwardInput = 0.0f;
        if (glfwGetKey(gameWindow, gameSettings.keyBindForward.keyCode) == GLFW_PRESS)
        {
            leftForwardInput += 1.0f;
        }
        if (glfwGetKey(gameWindow, gameSettings.keyBindBackward.keyCode) == GLFW_PRESS)
        {
            leftForwardInput += -1.0f;
        }
        controller.lStickY = leftForwardInput;


        float leftSideInput = 0.0f;
        if (glfwGetKey(gameWindow, gameSettings.keyBindRight.keyCode) == GLFW_PRESS)
        {
            leftSideInput += 1.0f;
        }
        if (glfwGetKey(gameWindow, gameSettings.keyBindLeft.keyCode) == GLFW_PRESS)
        {
            leftSideInput += -1.0f;
        }
        controller.lStickX = leftSideInput;


        setKey(controller.jump, gameSettings.keyBindJump.keyCode, gameWindow);
        setKey(controller.talk, gameSettings.keyBindTalk.keyCode, gameWindow);
        setKey(controller.pause, gameSettings.keyBindExit.keyCode, gameWindow);
        setKey(controller.debugBreak, gameSettings.keyBindBreak.keyCode, gameWindow);
    }
}
