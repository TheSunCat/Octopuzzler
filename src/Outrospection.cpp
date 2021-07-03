#include "Outrospection.h"

#include <csignal>
#include <glm/ext/matrix_clip_space.hpp>

#include "Util.h"
#include "Core/Layer.h"
#include "Core/UI/GUIControlsOverlay.h"

#include "Core/UI/GUILayer.h"
#include "Core/UI/GUIOctopusOverlay.h"
#include "Core/UI/GUIPause.h"
#include "Core/UI/GUIScene.h"
#include "Core/UI/GUIWelcome.h"
#include "Core/UI/GUIWinOverlay.h"
#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"


int WINDOW_WIDTH = 1920 / 1.5;
int WINDOW_HEIGHT = 1080 / 1.5;

int CRT_WIDTH = 256;
int CRT_HEIGHT = 192;

int SCR_WIDTH = WINDOW_WIDTH;
int SCR_HEIGHT = WINDOW_HEIGHT;

void updateRes()
{
    SCR_WIDTH = WINDOW_WIDTH;
    SCR_HEIGHT = WINDOW_HEIGHT;
}

Outrospection* Outrospection::instance = nullptr;

Outrospection::Outrospection()
{
    instance = this;

    preInit = PreInitialization();

    gameWindow = opengl.gameWindow;
    crtVAO = opengl.crtVAO;
    crtFramebuffer = opengl.framebuffer;
    textureColorbuffer = opengl.textureColorbuffer;

    fontCharacters = freetype.loadedCharacters;

    registerCallbacks();
    createShaders();
    createCursors();
    createIcon();

    // TODO check if we can access files, if not warn the user

    glfwSetCursor(gameWindow, cursorNone);
    
    octopusOverlay = new GUIOctopusOverlay();
    controlsOverlay = new GUIControlsOverlay();
    welcomeOverlay = new GUIWelcome();
    winOverlay = new GUIWinOverlay();
    scene = new GUIScene();

    pushLayer(scene);
    pushOverlay(octopusOverlay);
    pushOverlay(controlsOverlay);
    pushOverlay(welcomeOverlay);

    //soundEngine->play2D("./res/SoundData/totallyNotABossBattle.ogg", true);
}

Outrospection::~Outrospection()
{
    LOG_INFO("Terminating engine...");

    glfwTerminate();
    //timeThread.request_stop();
    //timeThread.join();

    //consoleThread.request_stop();
    //consoleThread.join();

    loggerThread.request_stop();
    loggerThread.join();

    std::cout << "Terminated the termination of the engine." << std::endl;
}

void Outrospection::stop()
{
    running = false;
}

void Outrospection::run()
{
    running = true;
    
    startLoggerThread();
    // TODO startConsoleThread();

    lastFrame = Util::currentTimeMillis();
    while (running)
    {
        runGameLoop();

        if (glfwWindowShouldClose(gameWindow))
            running = false;
    }
}

void Outrospection::onEvent(Event& e)
{
    dispatchEvent<WindowCloseEvent>(e, BIND_EVENT_FUNC(Outrospection::onWindowClose));
    //dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnWindowResize));
    dispatchEvent<MouseMovedEvent>(e, BIND_EVENT_FUNC(Outrospection::onMouseMoved));
    //dispatchEvent<MouseScrolledEvent>(e, BIND_EVENT_FUNC(Outrospection::onMouseScrolled));

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

void Outrospection::popLayer(Layer* layer)
{
    layerStack.popLayer(layer);
    layer->onDetach();
}

void Outrospection::popOverlay(Layer* overlay)
{
    layerStack.popOverlay(overlay);
    overlay->onDetach();
}

void Outrospection::captureMouse(const bool doCapture)
{
    if (false) { // TODO not hardcode this
        glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        //glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        //glfwSetCursorPos(gameWindow, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);
    }
}

void Outrospection::setEye(Eye letter)
{
    eye = letter;

    switch(eye)
    {
    case Eye::NONE:
        glfwSetCursor(gameWindow, cursorNone);
        break;
    case Eye::CIRCLE:
        glfwSetCursor(gameWindow, cursorCircle);
        break;
    case Eye::SQUARE:
        glfwSetCursor(gameWindow, cursorSquare);
        break;
    case Eye::TRIANGLE:
        glfwSetCursor(gameWindow, cursorTriangle);
        break;
    }
    
}

Eye Outrospection::getEye() const
{
    return eye;
}

void Outrospection::scheduleWorldTick()
{
    lastTick = Util::currentTimeMillis() - 5000;
}

void Outrospection::runGameLoop()
{
    currentTimeMillis = Util::currentTimeMillis();

    const auto currentFrame = currentTimeMillis;
    deltaTime = float(currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    if(deltaTime == 0.0f) // first frame will be 0. Assume it was 60fps
    {
        deltaTime = 1.0f / 60.0f; 
    }
    
    // Update game world
    {
        // fetch input into simplified controller class
        updateInput();

        if (!isGamePaused)
        {
            // Run one "tick" of the game physics
            runTick();
            textureManager.tickAllTextures();

            // execute scheduled tasks
            for(int i = 0; i < futureFunctions.size(); i++)
            {
                const auto& futureFunc = futureFunctions[i];
                
                if(currentTimeMillis - futureFunc.startTime > futureFunc.waitTime)
                {
                    futureFunc.func();

                    // pop the function
                    futureFunctions.erase(futureFunctions.begin() + i);
                    i--;
                }
            }
        }
        
        // UIs are also updated when game is paused
        for (auto& layer : layerStack)
        {
            layer->tick();
        }
    }

    // Draw the frame!
    {
        glDisable(GL_DEPTH_TEST); // disable depth test so stuff near camera isn't clipped

        
        glBindFramebuffer(GL_FRAMEBUFFER, crtFramebuffer);
        SCR_WIDTH = CRT_WIDTH; SCR_HEIGHT = CRT_HEIGHT;
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.3725, 0.4667, 0.5529f, 1.0f); // clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // draw stuff here
        if(!won)
            scene->draw();


        // bind to default framebuffer and draw custom one over that
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        SCR_WIDTH = WINDOW_WIDTH; SCR_HEIGHT = WINDOW_HEIGHT;
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // apply CRT effect
        crtShader.use();
        crtShader.setVec2("resolution", CRT_WIDTH, CRT_HEIGHT);
        crtShader.setFloat("time", float(currentTimeMillis % 1000000) / 1000000);
        
        glBindVertexArray(crtVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);    // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        screenShader.use();
        // draw UI
        for (const auto& layer : layerStack)
        {
            if (layer->handleManually)
                continue;
            
            layer->draw();
        }
    }

    // check for errors
    Util::glError();

    // swap buffers and poll IO events
    // -------------------------------
    glfwSwapBuffers(gameWindow);
    glfwPollEvents();
}

void Outrospection::runTick()
{
    if (currentTimeMillis - lastTick < 200) // five ticks per second
        return;
    
    lastTick = currentTimeMillis;

    ((GUIScene*)scene)->worldTick();
}

void Outrospection::registerCallbacks() const
{
    // Register OpenGL events
    glfwSetFramebufferSizeCallback(gameWindow, [](GLFWwindow*, const int width, const int height)
    {
        SCR_WIDTH = width; SCR_HEIGHT = height;
        updateRes();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
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

    glfwSetKeyCallback(gameWindow, key_callback);
    glfwSetErrorCallback(error_callback);
}

void Outrospection::createShaders()
{
    screenShader          = Shader("screen"   , "screen"         );
    crtShader             = Shader("crt"      , "crt"            );
    spriteShader          = Shader("sprite"   , "sprite"         );
    glyphShader           = Shader("sprite"   , "glyph"          );

    // set up 2d shader
    const glm::mat4 projection = glm::ortho(0.0f, float(SCR_WIDTH),
                                            float(SCR_HEIGHT), 0.0f, -1.0f, 1.0f);
    spriteShader.use();
    spriteShader.setMat4("projection", projection);

    glyphShader.use();
    glyphShader.setMat4("projection", projection);
}

void Outrospection::createCursors()
{
    GLFWimage cursorImage;
    int width = 10, height = 10;

    unsigned char* data = TextureManager::readImageBytes("./res/ObjectData/Textures/mouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorNone = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("./res/ObjectData/Textures/circleMouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorCircle = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("./res/ObjectData/Textures/squareMouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorSquare = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("./res/ObjectData/Textures/triangleMouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorTriangle = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);
}

void Outrospection::createIcon() const
{
    GLFWimage image;
    int width = 10, height = 10;

    unsigned char* data = TextureManager::readImageBytes("./res/ObjectData/icon.png", width, height);
    image.pixels = data; image.width = width; image.height = height;

    glfwSetWindowIcon(gameWindow, 1, &image);

    TextureManager::free(data);
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

    return false;
}

void Outrospection::scroll_callback(GLFWwindow*, const double xoffset, const double yoffset)
{
    
}

// this function is called when you press a key
void Outrospection::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        LOG_INFO("pressed key %i", key);

        switch(key)
        {
        case GLFW_KEY_Z:
            LOG("Pressed CIRCLE");
            break;
        case GLFW_KEY_X:
            LOG("Pressed SQUARE");
            break;
        case GLFW_KEY_C:
            LOG("Pressed TRIANGLE");
            break;
        case GLFW_KEY_ESCAPE:
            Outrospection::get().running = false;
            break;
        }
    }
}

void Outrospection::error_callback(const int errorcode, const char* description)
{
    LOG_ERROR("GLFW error (%i): %s", errorcode, description);
}

void Outrospection::updateInput()
{
    
}

void Outrospection::startLoggerThread()
{
    loggerThread = std::jthread{
        [&](std::stop_token stopToken) -> void
        {
            std::cout << "Started logger thread." << std::endl;

            while (!stopToken.stop_requested()) {
                while (!loggerQueue.empty())
                {
                    const auto& log = loggerQueue.pop();
                    if (log != nullptr)
                    {
                        log();
                        std::putchar('\n');
                    }
                }

                std::this_thread::yield();
            }

            std::cout << "Stopped logger thread." << std::endl;
        }
    };
}

void Outrospection::startConsoleThread()
{
    consoleThread = std::jthread{
        [&](std::stop_token stopToken) -> void
        {
            std::cout << "Started console thread." << std::endl;

            while (!stopToken.stop_requested()) {
                char input[512];

                std::cin.getline(input, sizeof(input));

                size_t size = strlen(input);

                if (size > 0)
                {
                    char start = input[0];
                    if (start == '/') // is a command
                    {
                        std::string_view rawCommand = &input[1];

                        std::vector<std::string_view> args;

                        size_t spaceIndex = 0;// rawCommand.find(' ');
                        while ((spaceIndex = rawCommand.find(' ', spaceIndex + 1)) != std::string::npos)
                        {
                            args.push_back(rawCommand.substr(spaceIndex + 1, rawCommand.find_first_of(' ', spaceIndex + 1)));
                        }

                        std::string_view command = rawCommand.substr(0, rawCommand.find(' '));



                        if (command == "hello")
                        {
                            LOG_INFO("Hi!!!!");
                        }
                        else if (command == "help")
                        {
                            LOG_INFO("Here is a list of commands:");
                            LOG_INFO("/hello");
                        }
                        else {
                            LOG_ERROR("Unknown command %s! Try /help", input);
                        }

                    }
                    else // is a chat message
                    {
                        LOG("<John> %s", input);
                    }
                }

                std::this_thread::yield();
            }

            std::cout << "Stopped console thread." << std::endl;
        }
    };
}