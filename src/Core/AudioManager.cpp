#include "AudioManager.h"

//#include <Core.h>

AudioManager::AudioManager()
{
    ma_result result;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("ERROR: Failed to initialize audio engine.");
        return;
    }
}

AudioManager::~AudioManager()
{
    ma_engine_uninit(&engine);
}

bool AudioManager::PlaySoundFile(const std::string& file, unsigned long vol)
{
    ma_result result = ma_engine_play_sound(&engine, file.c_str(), NULL);
    
    if(result != MA_SUCCESS)
    {
        printf("ERROR: Failed to play sound %s", file.c_str());
        return false;
    }
    
    return true;
}