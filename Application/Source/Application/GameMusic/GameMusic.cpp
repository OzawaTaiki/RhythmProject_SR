#include "GameMusic.h"

#include <System/Audio/AudioSystem.h>

#include <Application/EventData/PauseActionData.h>
#include <Features/Event/EventManager.h>

using namespace Engine;


GameMusic::GameMusic(const std::string& musicFilePath)
{
    soundInstance_ = AudioSystem::GetInstance()->Load(musicFilePath);
}

GameMusic::~GameMusic()
{
    if (voiceInstance_)
    {
        voiceInstance_.reset();
    }
}

void GameMusic::Initialize(float rewindTime)
{
    // コールバックの設定
    voiceCallBack_ = std::make_unique<VoiceCallBack>();
    voiceCallBack_->SetOnStreamEndCallback([this]()
                                           {
                                               MusicEnd();
                                               EventManager::GetInstance()->DispatchEvent(GameEvent("MusicEnded", nullptr));
                                           }); // 音楽が終了したときのコールバックを設定

    rewindTime_ = rewindTime;
    pausedAtTime_ = 0.0f;
    isMusicPlaying_ = false;

}

float GameMusic::GetElapsedTime() const
{
    if (voiceInstance_)
    {
        return voiceInstance_->GetElapsedTime();
    }
    return 0.0f; // voiceInstanceがない場合は0を返す
}

void GameMusic::Play(float volume)
{
    if (isMusicPlaying_)
        return;

    if (voiceInstance_)
        voiceInstance_.reset();

    voiceInstance_ = soundInstance_->Play(volume, 0, false, true, voiceCallBack_.get());

    isMusicPlaying_ = true;

}

void GameMusic::Resume()
{
    if (voiceInstance_ && !isMusicPlaying_)
    {
        voiceInstance_->Play();
        isMusicPlaying_ = true;
    }
}

void GameMusic::ResumeWithRewind(float volume)
{
    if (voiceInstance_ && !isMusicPlaying_)
    {
        voiceInstance_.reset();

        // 巻き戻し再生の開始時間を計算
        float startTime = pausedAtTime_ - rewindTime_;
        startTime = (std::max)(startTime, 0.0f); // 負の値にならないようにする

        // 再生を開始
        voiceInstance_ = soundInstance_->Play(volume, startTime, false, true, voiceCallBack_.get());
        isMusicPlaying_ = true;
    }
}

void GameMusic::Pause()
{
    if (voiceInstance_ && isMusicPlaying_)
    {
        pausedAtTime_ = voiceInstance_->GetElapsedTime(); // 現在の再生時間を記録

        voiceInstance_->Pause();
        isMusicPlaying_ = false;
    }
}

void GameMusic::SetVolume(float volume)
{
    if (voiceInstance_)
    {
        voiceInstance_->SetVolume(volume);
    }
}

float GameMusic::GetDuration() const
{
    if (soundInstance_)
    {
        return soundInstance_->GetDuration();
    }

    return 0.0f; // soundInstanceがない場合は0を返す
}

std::shared_ptr<VoiceInstance> GameMusic::GetVoiceInstance()
{
    if (voiceInstance_)
        return voiceInstance_;

    return nullptr;
}

std::shared_ptr<SoundInstance> GameMusic::GetSoundInstance()
{
    if (soundInstance_)
        return soundInstance_;
    return nullptr;
}
