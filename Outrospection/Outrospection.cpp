﻿#include "Outrospection.h"

#include <csignal>
#include <glm/ext/matrix_clip_space.hpp>

#include "irrKlang/irrKlang.h"

#include "Util.h"
#include "Core/Layer.h"
#include "Core/UI/GUIControlsOverlay.h"

#include "Core/UI/GUILayer.h"
#include "Core/UI/GUIOctopusOverlay.h"
#include "Core/UI/GUIPause.h"
#include "Core/UI/GUIScene.h"
#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"


int WINDOW_WIDTH = 1920 / 1.5;
int WINDOW_HEIGHT = 1080 / 1.5;

int CRT_WIDTH = WINDOW_WIDTH / 2;
int CRT_HEIGHT = WINDOW_HEIGHT / 1.5;

int SCR_WIDTH = WINDOW_WIDTH;
int SCR_HEIGHT = WINDOW_HEIGHT;

void updateRes()
{
    CRT_WIDTH = WINDOW_WIDTH / 2;
    CRT_HEIGHT = WINDOW_HEIGHT / 1.5;

    SCR_WIDTH = WINDOW_WIDTH;
    SCR_HEIGHT = WINDOW_HEIGHT;
}

Outrospection* Outrospection::instance = nullptr;

irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

Outrospection::Outrospection()
{
    instance = this;

    preInit = PreInitialization();

    gameWindow = opengl.gameWindow;
    crtVAO = opengl.crtVAO;
    crtFramebuffer = opengl.framebuffer;
    textureColorbuffer = opengl.textureColorbuffer;

    //initCrtVAO();

    fontCharacters = freetype.loadedCharacters;

    registerCallbacks();
    createShaders();
    createCursors();
    

    glfwSetCursor(gameWindow, cursorNone);

    std::string level0 =  "\
wwwwwwwww\
wwww   ww\
wwww w ww\
w   ww ww\
www    ww\
wwwwwwwww";
	
    scene = new GUIScene(level0, 8);
    octopusOverlay = new GUIOctopusOverlay();
    controlsOverlay = new GUIControlsOverlay();

    pushLayer(scene);
    pushOverlay(octopusOverlay);
    pushOverlay(controlsOverlay);

    soundEngine->play2D("./res/ObjectData/Sounds/taco.ogg", true);
}

void Outrospection::run()
{
    running = true;

    startTimeThread();
    startLoggerThread();
    startConsoleThread();
    //startDiscordThread();

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
    if (doCapture) {
        glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetCursorPos(gameWindow, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);
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

bool Outrospection::controlBound(Control control)
{
    for(KeyBinding& bind : keyBinds)
    {
        if (bind.m_control == control)
            return true; // can't bind control twice!
    }

    return false;
}

void Outrospection::doControl(Eye pokedEye)
{
	for(KeyBinding& bind : keyBinds)
	{
		if(bind.m_eye == pokedEye)
		{
            inputQueue.emplace_back(bind.m_control);
		}
	}
}

void Outrospection::runGameLoop()
{
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

        // TODO remove this LOG("%f, %f", lastMousePos.x, lastMousePos.y);
    	
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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw stuff here
        scene->draw();


        // bind to default framebuffer and draw custom one over that
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        SCR_WIDTH = WINDOW_WIDTH; SCR_HEIGHT = WINDOW_HEIGHT;
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    	
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
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
    if (inputQueue.empty())
        return;

    if (currentTimeMillis - lastTick < 1000) // one tick per second
        return;
	
    lastTick = currentTimeMillis;

	// read input
    Control curInput = inputQueue[0];
    LOG_INFO("Player is playing %i", int(curInput));

	// erase it so we move to the next one
    inputQueue.erase(inputQueue.begin());
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
    screenShader    = Shader("screen"   , "screen"   );
    crtShader       = Shader("crt"      , "crt"      );
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

void Outrospection::createCursors()
{
    GLFWimage cursorImage;
    int width = 10, height = 10;

    unsigned char* data = TextureManager::readImageBytes("./res/ObjectData/Textures/cursorNone.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorNone = glfwCreateCursor(&cursorImage, 0, 0);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("./res/ObjectData/Textures/cursorCircle.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorCircle = glfwCreateCursor(&cursorImage, 32, 32);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("./res/ObjectData/Textures/cursorSquare.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorSquare = glfwCreateCursor(&cursorImage, 32, 32);
    TextureManager::free(data);

    data = TextureManager::readImageBytes("./res/ObjectData/Textures/cursorTriangle.png", width, height);
    cursorImage.pixels = data; cursorImage.width = width; cursorImage.height = height;
    cursorTriangle = glfwCreateCursor(&cursorImage, 32, 32);
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

void Outrospection::startTimeThread()
{
    timeThread = std::thread{
        [&]() -> void
        {
            while (true)
            {
                auto now = std::chrono::system_clock::now();

                currentTimeSeconds = std::chrono::system_clock::to_time_t(now);
                currentTimeMillis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

                std::this_thread::yield();
            }
        }
    };
}

void Outrospection::startLoggerThread()
{
    loggerThread = std::thread{
        [&]() -> void
        {
            while (true)
            {
                const auto& log = loggerQueue.pop();
                if (log != nullptr)
                {
                    log();
                    std::putchar('\n');
                }

                std::this_thread::yield();
            }
        }
    };
}

void Outrospection::startConsoleThread()
{
    consoleThread = std::thread {
        [&]() -> void
        {
            char input[512];

            while (true)
            {
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
                        else if(command == "help")
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
            }
        }
    };
}

volatile bool discordInterrupted{ false };

void Outrospection::startDiscordThread()
{
    discordThread = std::thread {
        [&]() -> void {
            discord::Core* discordCore;
        	
            auto result = discord::Core::Create(793250268099641386, DiscordCreateFlags_Default, &discordCore);

            if (!discordCore)
            {
                LOG_ERROR("Discord: Failed to instantiate Discord core! Error %i", int(result));
                return;
            }

            discordCore->SetLogHook(discord::LogLevel::Debug, [](discord::LogLevel level, const char* message)
            {
                LOG("Discord: Log(%i): %s", uint32_t(level), message);
            });

            discord::Activity activity{};
            activity.SetDetails("Ingame");
            activity.SetState("Marching cubes");
            //activity.GetAssets().SetSmallImage("smallimage");
            //activity.GetAssets().SetSmallText("smalltext");
            activity.GetAssets().SetLargeImage("fishlady");
            // activity.GetAssets().SetLargeText("Oh, hi there!");
            activity.SetType(discord::ActivityType::Playing);

            discordCore->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
                LOG("%s to update activity!", (result == discord::Result::Ok) ? "Succeeded" : "Failed");
            });
            
            
        	
            std::signal(SIGINT, [](int) {
                discordInterrupted = true;
            });
            do {
                discordCore->RunCallbacks();
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            } while (!discordInterrupted);
        }
    };
}
