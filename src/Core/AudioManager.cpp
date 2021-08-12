#include "AudioManager.h"

#include "Util.h"

void AudioManager::loadSound(const std::string& soundName)
{
    auto& [key, wave] = *waves.try_emplace(soundName).first;
    if (!wave) {
        wave = std::make_unique<SoLoud::Wav>();

        std::string file = "res/SoundData/" + soundName + ".ogg";

        wave->load(file.c_str()); // load the file
    }
}

void AudioManager::init(const std::vector<std::string>& sounds)
{
    engine.init(0U, // aFlags
                0U, // aBackend
                0U, // aSampleRate
                0U, // aBufferSize
                1U);// aChannels

    for(const std::string& sound : sounds)
        loadSound(sound);
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
    
    wave->setLooping(loop);

    LOG("Playing sound %s", soundName);
    handles.insert_or_assign(soundName, engine.play(*wave, vol));
}

void AudioManager::setSoundVolume(const std::string& sound, float vol)
{
    auto f = handles.find(sound); // <- I'm getting the error here
    if (f == handles.end()) {
        LOG_ERROR("Tried to change volume of nonexistent sound \"%s\"!", sound);
        return;
    }

    engine.setVolume(f->second, vol);
}

void AudioManager::setGlobalVolume(float vol)
{
    engine.setGlobalVolume(vol);
}