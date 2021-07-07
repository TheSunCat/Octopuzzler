#pragma once

#include <string>
#include <vector>
#include <list>
#include <thread>

#include <External/miniaudio.h>
#include <External/miniaudio_engine.h>

#include "Core.h"

class AudioManager
{
public:
    ma_engine engine;

    std::jthread audioThread;

    // TODO make this private
    Queue<std::string> audioQueue;
    bool playNow(const std::string& soundName, float vol = 1);
//public:
    AudioManager();
    ~AudioManager();

    void play(const std::string& soundName, float vol = 1);

};