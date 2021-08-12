#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Core.h"
#include "Util.h"

class Framebuffer
{
    bool isDefaultFramebuffer = true;

    GLuint id = 0;
    GLuint texId = 0;
    GLuint rbo = 0;
public:
    Framebuffer() = default;
    Framebuffer(int width, int height);

    void bind();
    void bindTexture();

    void scaleResolution(float scale);

    glm::ivec2 defaultResolution = glm::ivec2(1920, 1080);
    glm::ivec2 resolution = glm::ivec2(1920, 1080);

    //DISALLOW_COPY_AND_ASSIGN(Framebuffer)
};
