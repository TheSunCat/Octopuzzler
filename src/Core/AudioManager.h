#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <list>
#include <thread>

//#define WITH_MINIAUDIO
#include "soloud.h"
#include "soloud_wav.h"

#include "Core.h"

class AudioManager
{
private:
    SoLoud::Soloud engine;
    std::unordered_map<std::string, std::unique_ptr<SoLoud::Wav>> waves;
public:
    AudioManager() = default;
    ~AudioManager();

    void init();
    void play(const std::string& soundName, float vol = 1.0f, bool loop = false);
};