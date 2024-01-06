#include "Outrospection.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <csignal>
#include <Core/UI/GUIBackground.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "GLFW/glfw3.h"
#include "Util.h"
#include "Core/Layer.h"
#include "Core/UI/GUIControlsOverlay.h"

#include "Core/UI/GUILayer.h"
#include "Core/UI/GUIOctopusOverlay.h"
#include "Core/UI/GUIProgressBar.h"
#include "Core/UI/GUIScene.h"
#include "Core/UI/GUIGuide.h"
#include "Core/UI/GUIWinOverlay.h"
#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

Outrospection* Outrospection::instance = nullptr;

Outrospection::Outrospection(bool speedrun)
{
    instance = this;

    loggerThread.start();
    // TODO consoleThread.start();

    if(speedrun)
        setSpeedrun();

    preInit = PreInitialization();
    audioManager.init({ "Control_Select", "Eye_Poke_0", "Eye_Poke_1", "Eye_Poke_2", "Flag_Get", "Mic_Off", "Mic_On", "Movement", "totallyNotABossBattle", "Waffle_Get" });

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
    
    background = new GUIBackground();
    progressBarOverlay = new GUIProgressBar();
    octopusOverlay = new GUIOctopusOverlay();
    controlsOverlay = new GUIControlsOverlay();
    guideOverlay = new GUIGuide();
    winOverlay = new GUIWinOverlay();
    scene = new GUIScene();

    pushLayer(scene);
    pushOverlay(background);
    pushOverlay(progressBarOverlay);
    pushOverlay(octopusOverlay);
    pushOverlay(guideOverlay);
    pushOverlay(controlsOverlay);

    audioManager.play("totallyNotABossBattle", 1, true);

    // for good measure, redo UI here
    int width = 0, height = 0;
    glfwGetFramebufferSize(gameWindow, &width, &height);
    updateResolution(width, height);
}

Outrospection::~Outrospection()
{
    LOG_INFO("Terminating engine...");

    glfwTerminate();

    //consoleThread.stop();
    loggerThread.stop();

    std::cout << "Terminated the termination of the engine." << std::endl;
}

void Outrospection::setSpeedrun()
{
    speedrunMode = true;
    
    LOG_INFO("Speedrun mode enabled.");
}

bool Outrospection::isSpeedrun() const
{
    return speedrunMode;
}

void Outrospection::stop()
{
    running = false;
}

void Outrospection::run()
{
    using namespace std::chrono_literals;

    running = true;

    lastFrame = Util::currentTimeMillis(); // I miss java
    deltaTime = 1.0f / 60.0f;

    while (running)
    {
        currentTimeMillis = Util::currentTimeMillis();
        deltaTime = float(currentTimeMillis - lastFrame) / 1000.0f;
        lastFrame = currentTimeMillis;

        runGameLoop();

        if (glfwWindowShouldClose(gameWindow))
            running = false;



        currentTimeMillis = Util::currentTimeMillis();
        time_t frameTime = currentTimeMillis - lastFrame;

        // sleep for any extra time we have
        auto extraTime = 16 - frameTime;
        //LOG("%i", extraTime);

        if(extraTime > 0) {
            auto m = std::chrono::milliseconds(extraTime - 1);
            std::this_thread::sleep_for(m);
        }
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

void Outrospection::toggleFullscreen()
{
    auto monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (isFullscreen)
    { // exit fullscreen
        int w = mode->width;
        int h = mode->height;

        int windowWidth = w / 2;
        int windowHeight = h / 2;

        glfwSetWindowMonitor(gameWindow, nullptr,
            (w - windowWidth) / 2, (h - windowHeight) / 2,
            windowWidth, windowHeight, mode->refreshRate);
        updateResolution(windowWidth, windowHeight);
    } else
    { // enter fullscreen
        
        glfwSetWindowMonitor(gameWindow, monitor, 0, 0,
            mode->width, mode->height, mode->refreshRate);
        updateResolution(mode->width, mode->height);
    }

    isFullscreen = !isFullscreen;
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(!won)
            scene->draw();


        framebuffers["default"].bind();
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
        Outrospection::get().updateResolution(width, height);
    });

    glfwSetCursorPosCallback(gameWindow, [](GLFWwindow* window, const double xPosD, const double yPosD)
    {
        // support for HiDPI
        float xDPI = 0, yDPI = 0;
        glfwGetWindowContentScale(window, &xDPI, &yDPI);


        glm::ivec2 windowRes = Outrospection::get().getWindowResolution();
        float targetAspectRatio = 1920 / 1080.f;

        float width = windowRes.x;
        float height = (width / targetAspectRatio + 0.5f);

        if (height > windowRes.y) // pillarbox
        {
            height = windowRes.y;
            width = (height * targetAspectRatio + 0.5f);
        }

        // weird center but it works
        float xPos = float(xPosD) - (windowRes.x - width) / (2 * xDPI);
        float yPos = float(yPosD) - (windowRes.y - height) / (2 * yDPI);

        float scaleFactor = width / 1920.f;
        float scaledX = xPos * (1/scaleFactor);
        float scaledY = yPos * (1/scaleFactor);

        scaledX *= xDPI;
        scaledY *= yDPI;

        MouseMovedEvent event(scaledX, scaledY);
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

    screenShader = Shader("screen", "screen");
    crtShader    = Shader("crt"   , "crt"   );
    spriteShader = Shader("sprite", "sprite");
    inkShader    = Shader("sprite", "ink"   );
    glyphShader  = Shader("sprite", "glyph" );
}

void Outrospection::createCursors()
{
    GLFWimage cursorImage;
    int width = 10, height = 10;

    unsigned char* data = TextureManager::readImageBytes("res/ObjectData/Textures/mouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorNone = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("res/ObjectData/Textures/circleMouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorCircle = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("res/ObjectData/Textures/squareMouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorSquare = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("res/ObjectData/Textures/triangleMouse.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorTriangle = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);
}

void Outrospection::createIcon() const
{
    GLFWimage image;
    int width = 10, height = 10;

    unsigned char* data = TextureManager::readImageBytes("res/ObjectData/icon.png", width, height);
    image.pixels = data; image.width = width; image.height = height;

    glfwSetWindowIcon(gameWindow, 1, &image);

    TextureManager::free(data);
}

void Outrospection::setResolution(glm::vec2 res)
{
    updateResolution(res.x, res.y);
}

void Outrospection::updateResolution(int x, int y)
{
    curWindowResolution = glm::ivec2(x, y);

    LOG_INFO("updateResolution(%i, %i)", x, y);
}

glm::vec2 Outrospection::getWindowResolution() const
{
    return glm::vec2(curWindowResolution);
}

void Outrospection::setWindowText(const std::string& text) const
{
    glfwSetWindowTitle(gameWindow, ("Octopuzzler | " + text).c_str());
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

    std::cout << lastMousePos.x << ", " << lastMousePos.y << std::endl;

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
#ifdef _DEBUG
        case GLFW_KEY_ESCAPE:
            Outrospection::get().running = false;
            break;
#endif
        case GLFW_KEY_F11:
            Outrospection::get().toggleFullscreen();
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

int Outrospection::loadSave()
{
#ifdef _DEBUG // don't load save data on Debug mode
    return 0;
#endif

    if(Util::fileExists("save"))
    {
        std::string saveData = Util::readAllBytes("save");

        if(!saveData.starts_with("OSAV"))
        {
            LOG_ERROR("Save file signature invalid!");
            return 0;
        }

        saveData = saveData.substr(4);
        return Util::stoi(saveData);
    }

    return 0;
}

void Outrospection::writeSave(int number)
{
#ifdef _DEBUG
    return;
#endif

    if(Util::fileExists("save"))
    {
        std::remove("save");
    }

    std::ofstream saveFile("save");
    saveFile << "OSAV" << number;
    saveFile.close();
}
