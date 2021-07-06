#include "AudioManager.h"

//#include <Core.h>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    auto decoders = (std::list<ma_decoder>*)pDevice->pUserData;
    if (decoders == nullptr) {
        return;
    }

    printf("decoding\n");

    for (auto& decoder : *decoders) {
        ma_decoder_read_pcm_frames(&decoder, pOutput, frameCount);
    }

    (void)pInput;
}

AudioManager::AudioManager()
{
    ma_device_config deviceConfig;

    // not sure what to set here; the example uses data from an already-loaded mp3.
    // my API loads files after constructing the AudioManager
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    //deviceConfig.playback.format = newDecoder.outputFormat;
    deviceConfig.playback.channels = 2;// newDecoder.outputChannels;
    //deviceConfig.sampleRate = newDecoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoders;


    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        printf("ERROR: Failed to open playback device.\n");
        return;
    }


    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("ERROR: Failed to start playback device.\n");
        ma_device_uninit(&device);
        return;
    }
}

AudioManager::~AudioManager()
{
    ma_device_uninit(&device);

    for(auto& decoder : decoders)
    {
        ma_decoder_uninit(&decoder);
    }
}

bool AudioManager::PlaySoundFile(const std::string& file, unsigned long vol)
{
    ma_decoder newDecoder;

    ma_result result = ma_decoder_init_file(file.c_str(), nullptr, &newDecoder);
    if (result != MA_SUCCESS) {
        return false;
    }

    decoders.push_back(newDecoder);
    
    return true;
}