#include "AudioManager.h"

#include "Util.h"

void AudioManager::init()
{
    engine.init(1U, // aFlags
                0U, // aBackend
                0U, // aSampleRate
                1024U, // aBufferSize
                2U);// aChannels
}

AudioManager::~AudioManager()
{
    for(auto it = waves.begin(); it != waves.end(); ++it)
    {
        
    }

    engine.deinit();
}

void AudioManager::play(const std::string& soundName, float vol, bool loop)
{
    auto& [key, wave] = *waves.try_emplace(soundName).first;
    if (!wave) {
        wave = std::make_unique<SoLoud::Wav>();

        std::string file = "res/SoundData/" + soundName + ".ogg";

        wave->load(file.c_str()); // load the file
    }

    //wave->setVolume(vol);
    wave->setLooping(loop);

    LOG("Playing sound %s", soundName);
    engine.play(*wave, vol);
}