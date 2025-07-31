#include "GameMusic.h"

#include <System/Audio/AudioSystem.h>

#include <Application/EventData/PauseActionData.h>

GameMusic::GameMusic(const std::string& _musicFilePath)
{
    soundInstance_ = AudioSystem::GetInstance()->Load(_musicFilePath);
}

GameMusic::~GameMusic()
{
}

void GameMusic::Initialize(float _rewindTime)
{
    voiceCallBack_ = std::make_unique<VoiceCallBack>();
    voiceCallBack_->SetOnStreamEndCallback([this]() {MusicEnd(); }); // 音楽が終了したときのコールバックを設定

    rewindTime_ = _rewindTime;
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

void GameMusic::Play(float _volume)
{
    if (isMusicPlaying_)
        return;

    if (voiceInstance_)
        voiceInstance_.reset();

    voiceInstance_ = soundInstance_->Play(_volume, 0, false, true, voiceCallBack_.get());

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

void GameMusic::ResumeWithRewind(float _volume)
{
    if (voiceInstance_ && !isMusicPlaying_)
    {
        if (voiceInstance_)
            voiceInstance_.reset();

        float startTime = pausedAtTime_ - rewindTime_;
        startTime = (std::max)(startTime, 0.0f);

        voiceInstance_ = soundInstance_->Play(_volume, startTime, false, true, voiceCallBack_.get());
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

void GameMusic::SetVolume(float _volume)
{
    if (voiceInstance_)
    {
        voiceInstance_->SetVolume(_volume);
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
