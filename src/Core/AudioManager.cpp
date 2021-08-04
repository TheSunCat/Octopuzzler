#include "AudioManager.h"

#include "Util.h"

void AudioManager::init(const std::vector<std::string>& sounds)
{
    engine.init(0U, // aFlags
                0U, // aBackend
                0U, // aSampleRate
                0U, // aBufferSize
                1U);// aChannels

    // TODO make this not dumb
    // also code duplication
    for(auto sound : sounds)
    {
        auto& [key, wave] = *waves.try_emplace(sound).first;
        if (!wave) {
            wave = std::make_unique<SoLoud::Wav>();

            std::string file = "res/SoundData/" + sound + ".ogg";

            wave->load(file.c_str()); // load the file
        }
    }
}

AudioManager::~AudioManager()
{
    engine.deinit();
}

void AudioManager::play(const std::string& soundName, float vol, bool loop)
{
    auto& [key, wave] = *waves.try_emplace(soundName).first;
    if (!wave) {
        LOG_ERROR("Sound was played without being pre-loaded! Please add its name to the init call.");

        wave = std::make_unique<SoLoud::Wav>();

        std::string file = "res/SoundData/" + soundName + ".ogg";

        wave->load(file.c_str()); // load the file
    }

    //wave->setVolume(vol);
    wave->setLooping(loop);

    LOG("Playing sound %s", soundName);
    engine.play(*wave, vol);
}

void AudioManager::setGlobalVolume(float vol)
{
    engine.setGlobalVolume(vol);
}