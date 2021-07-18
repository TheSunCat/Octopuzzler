#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Core.h"
#include "Util.h"

class Framebuffer
{
    GLuint id = 0;
    GLuint texId = 0;
public:
    Framebuffer() = default;
    Framebuffer(int width, int height);

    void bind();
    void bindTexture();

    glm::vec2 resolution = glm::vec2(1920, 1080);

    //DISALLOW_COPY_AND_ASSIGN(Framebuffer)
};
