#pragma once

#include <System/Audio/SoundInstance.h>
#include <Features/Event/EventData.h>

/// <summary>
/// BGMChangeEventDataを表す構造体。
/// </summary>
struct BGMChangeEventData : Engine::EventData
{
    std::shared_ptr<Engine::SoundInstance> newBGM; // 新しいBGMのSoundInstance
};
