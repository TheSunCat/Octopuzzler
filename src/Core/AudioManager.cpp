#include "AudioManager.h"

AudioManager::AudioManager()
{
    ma_result result;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to initialize audio engine.");
        return;
    }

    audioThread = std::jthread{
        [this](std::stop_token stopToken) -> void
        {
            while (!stopToken.stop_requested()) {
                while (!this->audioQueue.empty())
                {
                    const auto& sound = this->audioQueue.pop();
                    //if (sound != nullptr)
                    {
                        this->playNow(sound);
                    }
                }

                // TODO sleep_for 1ms
                std::this_thread::yield();
            }
        }
    };
}

AudioManager::~AudioManager()
{
    audioThread.request_stop();
    audioThread.join();
    
    ma_engine_uninit(&engine);
}

void AudioManager::play(const std::string& soundName, float vol)
{
    audioQueue.push(soundName);
}

bool AudioManager::playNow(const std::string& soundName, float vol)
{
    std::string file = "./res/SoundData/" + soundName + ".mp3";

    LOG("Playing sound %s", soundName);

    // TODO this is really slow. Why?
    // look at miniaudio_engine.h documentation
    ma_result result = ma_engine_play_sound(&engine, file.c_str(), NULL);
    
    if(result != MA_SUCCESS)
    {
        LOG_ERROR("ERROR: Failed to play sound %s", file);
        return false;
    }

    return true;
}