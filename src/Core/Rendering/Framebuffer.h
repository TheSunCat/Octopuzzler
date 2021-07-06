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
public:
    Framebuffer() = default;
    Framebuffer(int width, int height);

    void bind();

    glm::vec2 resolution = Util::curResolution();

    //DISALLOW_COPY_AND_ASSIGN(Framebuffer)
};
