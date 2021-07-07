#pragma once

#include <string>
#include <vector>
#include <list>

#include <External/miniaudio.h>
#include <External/miniaudio_engine.h>

class AudioManager
{
    ma_engine engine;

public:
    AudioManager();
    ~AudioManager();

    bool PlaySoundFile(const std::string& file, unsigned long vol);
};