#pragma once

#include <System/Audio/SoundInstance.h>
#include <Features/Event/EventData.h>

struct BGMChangeEventData : Engine::EventData
{
    std::shared_ptr<Engine::SoundInstance> newBGM; // 新しいBGMのSoundInstance
};
