#pragma once

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Constants.h"
#include "Util.h"
#include "Framebuffer.h"

class OpenGL
{
public:
    OpenGL()
    {
        if (!glfwInit())
        {
            LOG_ERROR("GLFW has not initialized properly!");
            glfwTerminate();
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // add on Mac bc Apple is big dumb :(
#endif

        // Window init
        gameWindow = glfwCreateWindow(1280, 720, "Octopuzzler", nullptr, nullptr);
        if (gameWindow == nullptr)
        {
            LOG_ERROR("Failed to create GLFW window!");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(gameWindow);

        glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetWindowUserPointer(gameWindow, reinterpret_cast<void*>(this));

        // turn on VSync so we don't run at about a kjghpillion fps
        glfwSwapInterval(1);

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
