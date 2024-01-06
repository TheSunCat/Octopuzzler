#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Constants.h"
#include "Util.h"
#include "Framebuffer.h"

class OpenGL
{
public:
    GLFWwindow* createGameWindow(int width, int height, const std::string& name, GLFWmonitor* monitor, GLFWwindow* windowShare)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // add on Mac bc Apple is big dumb :(
#endif

        // Window init
        auto window = glfwCreateWindow(width, height, name.c_str(), monitor, windowShare);
        if (window == nullptr)
        {
            LOG_ERROR("Failed to create GLFW window!");
            glfwTerminate();
            return nullptr;
        }

        glfwMakeContextCurrent(window);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

        // turn on VSync so we don't run at about a kjghpillion fps
        glfwSwapInterval(1);

        return window;
    }

    OpenGL()
    {
        if (!glfwInit())
        {
            LOG_ERROR("GLFW has not initialized properly!");
            glfwTerminate();
            return;
        }

        auto monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        int w = mode->width;
        int h = mode->height;

        int windowWidth = w / 2;
        int windowHeight = h / 2;

        gameWindow = createGameWindow(windowWidth, windowHeight, "Octopuzzler", nullptr, nullptr);
        glfwSetWindowPos(gameWindow, (w - windowWidth) / 2, (h - windowHeight) / 2);

        // load OGL function pointers
        if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
        {
            LOG_ERROR("Failed to initialize GLAD!");
            return;
        }

        // GL Settings
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        // init framebuffer

        // quad that fills CRT space
        float quadVertices[] = {
            // positions 
            -0.5f, 0.66f,
            -0.5f, -0.6667f,
            0.5f, -0.6667f,

            -0.5f, 0.66f,
            0.5f, -0.6667f,
            0.5f, 0.66f,
        };

        // screen quad VAO
        GLuint quadVBO;
        glGenVertexArrays(1, &crtVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(crtVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        
        framebuffer = Framebuffer(640, 480);
    }

    GLuint crtVAO = 0;
    Framebuffer framebuffer;
    GLFWwindow* gameWindow{};

    DISALLOW_COPY_AND_ASSIGN(OpenGL)
};
