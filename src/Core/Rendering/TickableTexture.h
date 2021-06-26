#pragma once

#include <string>
#include <vector>

#include "SimpleTexture.h"

class TickableTexture : public SimpleTexture
{
public:
    TickableTexture(const std::vector<GLuint>& texIds, const std::string& _texPath, unsigned int _frameLength);

    void tick() override;

    void tick(unsigned int& stepCount);

    void nextFrame();

    void reset() override;
private:
    const std::vector<GLuint> textures;
    const unsigned int frameLength;

    unsigned int frameTally = 0;
    GLuint curFrame = 0;
};
