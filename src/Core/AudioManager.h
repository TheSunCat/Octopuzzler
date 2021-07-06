#pragma once

#include <string>
#include <vector>
#include <list>

#include <External/miniaudio.h>

class AudioManager
{
    ma_device device;

    std::list<ma_decoder> decoders;
    
public:
    AudioManager();
    ~AudioManager();

    bool PlaySoundFile(const std::string& file, unsigned long vol);
};