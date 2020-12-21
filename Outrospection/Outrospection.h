#pragma once
#include "Core.h"

#include <thread>
#include <glm/vec2.hpp>

#include "Constants.h"
#include "GameSettings.h"

#include "Controller.h"
#include "Util.h"
#include "Core/Camera.h"
#include "Core/LayerStack.h"
#include "Core/PreInitialization.h"
#include "Core/Registry.h"
#include "Core/Scene.h"
#include "Core/Rendering/FreeType.h"
#include "Core/Rendering/OpenGL.h"
#include "Core/Rendering/Shader.h"
#include "Core/Rendering/TextureManager.h"
#include "Core/UI/GUILayer.h"
#include "Core/World/PhysicsValues.h"
#include "Core/World/Player.h"
#include "Core/World/PlayerController.h"
#include "Item/Item.h"


class MouseMovedEvent;
class WindowCloseEvent;
class Event;
class Layer;

class Outrospection
{
    static void initItems();

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

    glm::vec2 lastMousePos = glm::vec2(SCR_HEIGHT / 2.0f, SCR_WIDTH / 2.0f);

    Scene scene;
    Player player;
    PlayerController playerController;
    TextureManager textureManager;
    GameSettings gameSettings;
    Controller controller{};

    std::unordered_map<char, FontCharacter> fontCharacters;

    Shader objectShader;
    Shader billboardShader;
    Shader skyShader;
    Shader screenShader;
    Shader simpleShader;
    Shader spriteShader;
    Shader glyphShader;

    inline static Registry<ItemID, Item> itemRegistry;
    inline static ItemStack noItem = ItemStack(0);

    DISALLOW_COPY_AND_ASSIGN(Outrospection)
private:
    void runGameLoop();
    void runTick();
    void updateCamera();

    GUILayer* ingameGUI;
    GUILayer* pauseGUI;
    GUILayer* inventoryGUI;

    // set to false when the game loop shouldn't run
    bool running = false;

    // timing
    float deltaTime = 0; // Time between current frame and last frame
    time_t lastFrame = 0; // Time of last frame

    GLFWwindow* gameWindow;

    GLuint framebuffer, intermediateFBO = 0;
    GLuint textureColorbuffer;
    GLuint quadVAO;

    // camera stuff
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    bool firstMouse = true;

    bool onWindowClose(WindowCloseEvent& e);
    bool onMouseMoved(MouseMovedEvent& e);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void error_callback(int errorcode, const char* description);

    void registerCallbacks() const;
    void createShaders();
    void updateInput();

    bool isGamePaused = false;

    bool doMoveCamera = true;
    

    LayerStack layerStack;

    static Outrospection* instance;

    std::thread timeThread{
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

    std::thread loggerThread{
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

    std::thread inputThread{
        [&]() -> void
        {
            char input[512];

            while(true)
            {
                std::cin.getline(input, sizeof(input));

                size_t size = strlen(input);

                if(size > 0)
                {
                    char start = input[0];
                    if(start == '/') // is a command
                    {
                        std::string_view rawCommand = &input[1];

                        std::vector<std::string_view> args;

                        size_t spaceIndex = 0;// rawCommand.find(' ');
                        while((spaceIndex = rawCommand.find(' ', spaceIndex + 1)) != std::string::npos)
                        {
                            args.push_back(rawCommand.substr(spaceIndex + 1, rawCommand.find_first_of(' ', spaceIndex + 1)));
                        }

                        std::string_view command = rawCommand.substr(0, rawCommand.find(' '));

                        

                        if(command == "gravity")
                        {
                            if(args.empty())
                            {
                                LOG("Gravity is %f.", Physics::gravityStrength);
                            }
                            else
                            {
                                float newGravity = Util::stof(args[0]);

                                Physics::gravityStrength = newGravity;

                                LOG("Set gravity to %f.", newGravity);
                            }
                        }
                        else if (command == "give")
                        {
                            if(!args.empty())
                            {
                                std::string_view itemToGive = args[0];

                                if (Util::isAllDigits(itemToGive))
                                {

                                    ItemID itemID = Util::stoi(itemToGive);

                                    if (!ITEM_EXISTS(itemID))
                                    {
                                        LOG_ERROR("give: item with ID %i does not exist in registry!", itemID);
                                    }
                                    else
                                    {
                                        unsigned int count = 1;
                                        if (args.size() >= 2)
                                            count = Util::stoi(args[1]);

                                        ItemStack stack(itemID);
                                        stack.setCount(count);

                                        Outrospection::get().player.inventory.addItem(stack);
                                    }
                                } else
                                {
                                    LOG_ERROR("give: please use numerical item IDs");
                                }
                            } else
                            {
                                LOG_ERROR("give: please provide item ID to give");
                            }
                        }
                        else if (command == "threshold")
                        {
                            if(!args.empty())
                            {
                                std::string_view threshold = args[0];

                                if (Util::isAllDigits(threshold, true))
                                {
                                    scene.cubeThreshold = Util::stof(threshold);

                                    LOG("New threshold: %f", scene.cubeThreshold);
                                }
                            }
                        } else if (command == "cubes")
                        {
                            if(!args.empty())
                            {
                                bool hidden = args[0] == "false";
                                

                                for (auto& cube : scene.objects["debugCubes"])
                                    cube.hidden = hidden;

                                LOG("Successfully hid/unhid cubes.");
                            }
                        }
                        else {
                            LOG_ERROR("Unknown command %s!", input);
                        }

                    } else // is a chat message
                    {
                        LOG("<John> %s", input);
                    }
                }
            }
        }
    };
};