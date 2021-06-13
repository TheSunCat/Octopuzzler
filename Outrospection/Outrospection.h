#pragma once
#include "Core.h"

#include <thread>
#include <glm/vec2.hpp>
#include "discord/discord.h"

#include "Types.h"
#include "Constants.h"

#include "KeyBinding.h"
#include "Util.h"
#include "Core/LayerStack.h"
#include "Core/PreInitialization.h"
#include "Core/Registry.h"
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
    constexpr static Outrospection& get()
    {
        return *instance;
    }

    Outrospection();

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
    bool controlBound(Control control);
    void doControl(Eye pokedEye);
    std::vector<KeyBinding> keyBinds;
    std::vector<Control> inputQueue;

    glm::vec2 lastMousePos = glm::vec2(SCR_HEIGHT / 2.0f, SCR_WIDTH / 2.0f);

    TextureManager textureManager;


	std::vector<Util::FutureRun> futureFunctions;
    std::unordered_map<char, FontCharacter> fontCharacters;
    
    Shader screenShader;
    Shader crtShader;
    Shader spriteShader;
    Shader glyphShader;

    DISALLOW_COPY_AND_ASSIGN(Outrospection)
private:
    void runGameLoop();
    void runTick();
    time_t lastTick;

    GUILayer* scene;
    GUILayer* octopusOverlay;
    GUILayer* controlsOverlay;

    // set to false when the game loop shouldn't run
    bool running = false;

    // timing
    float deltaTime = 0; // Time between current frame and last frame
    time_t lastFrame = 0; // Time of last frame

    GLFWwindow* gameWindow;

    GLuint crtFramebuffer, intermediateFBO = 0;
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
	
    Eye eye = Eye::NONE;
    GLFWcursor *cursorNone{}, *cursorCircle{}, *cursorSquare{}, *cursorTriangle{};
	
    void updateInput();

    bool isGamePaused = false;

    LayerStack layerStack;

    static Outrospection* instance;

    std::thread timeThread;
    void startTimeThread();

    std::thread loggerThread;
    void startLoggerThread();
    
    std::thread consoleThread;
    void startConsoleThread();
};