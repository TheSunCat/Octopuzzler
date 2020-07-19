#pragma once

#include <glm/glm.hpp>

#include "Core/Rendering/SimpleTexture.h"

class Shader;

class Billboard {
public:
    Billboard() = default;

    explicit Billboard(const SimpleTexture& _tex);

    void draw(const Shader& _shader, const glm::vec3& _pos) const;

    void draw(const Shader& _shader, const glm::vec3& _pos, const SimpleTexture& _tex) const;

private:
    unsigned int quadVAO = 0;
    SimpleTexture texture;
};