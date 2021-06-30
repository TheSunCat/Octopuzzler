#pragma once

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Constants.h"

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
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // add on Mac bc Apple is big dumb :(
#endif

        // Window init
        gameWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Octopuzzler", nullptr, nullptr);
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


        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // create a color attachment texture
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 192, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        GLuint RBO;
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 256, 192);
        // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
        // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint crtVAO = 0;
    GLuint framebuffer = 0;
    GLuint textureColorbuffer = 0;
    GLFWwindow* gameWindow{};

    DISALLOW_COPY_AND_ASSIGN(OpenGL)
};