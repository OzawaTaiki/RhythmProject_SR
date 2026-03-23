#include "JudgeSound.h"

#include <System/Audio/SoundEngine.h>

using namespace Engine;


JudgeSound::~JudgeSound()
{
    StopAllSounds();
}

void JudgeSound::Initialize(float volume)
{
    volume_ = volume;
    // サウンドのロードは SoundEngine::LoadSoundData() に委譲済み
}

void JudgeSound::Play()
{
    auto handle = SoundEngine::GetInstance()->Play("se_judge", volume_);
    if (handle != kInvalidHandle)
    {
        handles_.push_back(handle);
    }
}

void JudgeSound::CleanupStoppedVoices()
{
    auto* engine = SoundEngine::GetInstance();

    for (auto it = handles_.begin(); it != handles_.end();)
    {
        if (!engine->IsPlaying(*it))
            it = handles_.erase(it);
        else
            ++it;
    }
}

void JudgeSound::StopAllSounds()
{
    auto* engine = SoundEngine::GetInstance();

    for (auto handle : handles_)
    {
        engine->Stop(handle);
    }
    handles_.clear();
}
