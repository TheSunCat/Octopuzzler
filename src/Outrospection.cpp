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
#include "Core/UI/GUIGuide.h"
#include "Core/UI/GUIWinOverlay.h"
#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

Outrospection* Outrospection::instance = nullptr;

Outrospection::Outrospection() : curWindowResolution()
{
    instance = this;

    loggerThread.start();
    // TODO consoleThread.start();

    preInit = PreInitialization();
    audioManager.init();

    gameWindow = opengl.gameWindow;
    crtVAO = opengl.crtVAO;
    framebuffers.insert(std::make_pair("default", Framebuffer()));
    framebuffers.insert(std::make_pair("crt", opengl.framebuffer));

    fontCharacters = freetype.loadedCharacters;

    registerCallbacks();
    createShaders();
    createCursors();
    createIcon();

    glfwSetCursor(gameWindow, cursorNone);
    
    octopusOverlay = new GUIOctopusOverlay();
    controlsOverlay = new GUIControlsOverlay();
    guideOverlay = new GUIGuide();
    winOverlay = new GUIWinOverlay();
    scene = new GUIScene();

    pushLayer(scene);
    pushOverlay(octopusOverlay);
    pushOverlay(guideOverlay);
    pushOverlay(controlsOverlay);

    audioManager.play("totallyNotABossBattle", 1);
}

Outrospection::~Outrospection()
{
    LOG_INFO("Terminating engine...");

    glfwTerminate();

    //consoleThread.stop();
    loggerThread.stop();

    std::cout << "Terminated the termination of the engine." << std::endl;
}

void Outrospection::stop()
{
    running = false;
}

void Outrospection::run()
{
    using namespace std::chrono_literals;

    running = true;

    lastFrame = Util::currentTimeMillis();
    deltaTime = 1.0f / 60.0f; 
    while (running)
    {
        currentTimeMillis = Util::currentTimeMillis();

        const auto currentFrame = currentTimeMillis;
        deltaTime = float(currentFrame - lastFrame) / 1000.0f;
        lastFrame = currentFrame;

        runGameLoop();

        if (glfwWindowShouldClose(gameWindow))
            running = false;

        std::this_thread::sleep_for(1ms);
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
        
        framebuffers["crt"].bind();
        glClearColor(0.3725, 0.4667, 0.5529f, 1.0f); // clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(!won)
            scene->draw();


        framebuffers["default"].bind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw CRT with shader effect
        crtShader.use();
        
        glBindVertexArray(crtVAO);
        framebuffers["crt"].bindTexture();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        screenShader.use();
        // draw UI
        for (const auto& layer : layerStack)
        {
            if (layer->handleManually) // TODO this is jank
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
        LOG("Framebuffer size changed to %i, %i", width, height);

        Outrospection::get().setResolution(width, height);
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
    LOG_INFO("Setting up shaders...");

    screenShader          = Shader("screen"   , "screen"         );
    crtShader             = Shader("crt"      , "crt"            );
    spriteShader          = Shader("sprite"   , "sprite"         );
    glyphShader           = Shader("sprite"   , "glyph"          );

    // set up 2d shader
    //setResolution(getWindowResolution());
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

void Outrospection::setResolution(glm::vec2 res)
{
    setResolution(res.x, res.y);
}

void Outrospection::setResolution(int x, int y)
{
    glViewport(0, 0, x, y);
    curWindowResolution = glm::ivec2(x, y);

    for(auto& pair : framebuffers)
    {
        glm::vec2 scaleFactor = curWindowResolution / glm::vec2(1920, 1080);

        pair.second.scaleResolution(scaleFactor);
    }

    LOG_INFO("setResolution(%i, %i)", x, y);
}

glm::vec2 Outrospection::getWindowResolution()
{
    return glm::vec2(curWindowResolution);
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