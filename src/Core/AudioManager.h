#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <list>
#include <thread>
#include <vector>

#include "soloud.h"
#include "soloud_wav.h"

#include "Core.h"

class AudioManager
{
private:
    SoLoud::Soloud engine;
    std::unordered_map<std::string, std::unique_ptr<SoLoud::Wav>> waves;
    std::unordered_map<std::string, SoLoud::handle> handles;

    void loadSound(const std::string& soundName);
public:
    AudioManager() = default;
    ~AudioManager();

    void init(const std::vector<std::string>& sounds = std::vector<std::string>());
    void play(const std::string& soundName, float vol = 1.0f, bool loop = false);

    void setSoundVolume(const std::string& sound, float vol);
    void setGlobalVolume(float vol);
};