#pragma once
#include "Core.h"

#include <glm/vec2.hpp>

#include "Types.h"
#include "Constants.h"

#include "KeyBinding.h"
#include "Util.h"
#include "Core/jthread.h"
#include "Core/LayerStack.h"
#include "Core/PreInitialization.h"
#include "Core/Registry.h"
#include "Core/AudioManager.h"
#include "Core/Rendering/FreeType.h"
#include "Core/Rendering/OpenGL.h"
#include "Core/Rendering/Shader.h"
#include "Core/Rendering/TextureManager.h"
#include "Core/UI/GUILayer.h"


class MouseMovedEvent;
class WindowCloseEvent;
class Event;
class Layer;

class Outrospection
{
    PreInitialization preInit;

    OpenGL opengl; // defined at the beginning so nothing gets initialized before this
    FreeType freetype;

public:
    static Outrospection& get()
    {
        return *instance;
    }

    Outrospection();
    ~Outrospection();

    void stop();

    void run();
    void onEvent(Event& e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);

    void popLayer(Layer* layer);
    void popOverlay(Layer* overlay);

    void captureMouse(bool doCapture);

	// stores the letter and sets the mouse image
    void setEye(Eye letter);
    Eye getEye() const;

    void scheduleWorldTick(); // tick world NOW

    glm::vec2 lastMousePos = glm::vec2(Util::curResolution() / 2.0f);

    TextureManager textureManager;
    AudioManager audioManager;

	std::vector<Util::FutureRun> futureFunctions;
    std::unordered_map<char, FontCharacter> fontCharacters;
    
    Shader screenShader;
    Shader crtShader;
    Shader spriteShader;
    Shader glyphShader;

    GUILayer* scene;
    GUILayer* octopusOverlay;
    GUILayer* controlsOverlay;
    GUILayer* guideOverlay;
    GUILayer* winOverlay;

    bool won = false;

    time_t currentTimeMillis = 0;
	
    DISALLOW_COPY_AND_ASSIGN(Outrospection)
private:
    void runGameLoop();
    void runTick();
    time_t lastTick = 0;

    // set to false when the game loop shouldn't run
    bool running = false;

    // timing
    float deltaTime = 0; // Time between current frame and last frame
    time_t lastFrame = 0; // Time of last frame

    GLFWwindow* gameWindow;

    Framebuffer defaultFramebuffer;
    Framebuffer crtFramebuffer;
    GLuint textureColorbuffer;
    GLuint crtVAO;

    // camera stuff
    //Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    bool firstMouse = true;


    bool onWindowClose(WindowCloseEvent& e);
    bool onMouseMoved(MouseMovedEvent& e);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void error_callback(int errorcode, const char* description);
	
    void registerCallbacks() const;
    void createShaders();
    void createCursors();
    void createIcon() const;
	
    Eye eye = Eye::NONE;
    GLFWcursor *cursorNone{}, *cursorCircle{}, *cursorSquare{}, *cursorTriangle{};
	
    void updateInput();

    bool isGamePaused = false;

    LayerStack layerStack;

    static Outrospection* instance;

    jthread loggerThread{
        [&] {
            using namespace std::chrono_literals;

            while (!loggerQueue.empty())
            {
                const auto& log = loggerQueue.pop();
                if (log != nullptr)
                {
                    log();
                    std::putchar('\n');
                }
            }

            std::this_thread::sleep_for(1ms);
        }
    };
    
    jthread consoleThread{
        [&] {
            using namespace std::chrono_literals;

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

            std::this_thread::sleep_for(1ms);
        }
    };
};