#include "AudioController.h"

#include <System/Audio/AudioSystem.h>
#include <Debug/Debug.h>

namespace BME {

AudioController::AudioController()
{
    audioSystem_ = AudioSystem::GetInstance();
}

// ========================================
// ロード処理
// ========================================

bool AudioController::Load(const std::string& _filePath)
{
    if (!audioSystem_)
    {
        Debug::Log("AudioSystem is null\n");
        return false;
    }

    // 既存の音声を停止
    Stop();

    // 音声をロード
    musicSoundInstance_ = audioSystem_->Load(_filePath);

    if (!musicSoundInstance_)
    {
        Debug::Log("Failed to load audio: " + _filePath + "\n");
        return false;
    }

    Debug::Log("Audio loaded successfully: " + _filePath + "\n");
    return true;
}

// ========================================
// 再生制御
// ========================================

void AudioController::Play(float _startTime)
{
    if (!musicSoundInstance_)
    {
        Debug::Log("No audio loaded\n");
        return;
    }

    // 既存の再生を停止
    Stop();

    // 指定時間から再生
    musicVoiceInstance_ = musicSoundInstance_->Play(volume_, _startTime);

    if (musicVoiceInstance_)
    {
        musicVoiceInstance_->SetPlaySpeed(playSpeed_);
    }
}

void AudioController::Stop()
{
    if (musicVoiceInstance_)
    {
        musicVoiceInstance_->Stop();
        musicVoiceInstance_ = nullptr;
    }
}

void AudioController::Restart()
{
    Play(0.0f);
}

// ========================================
// 状態取得
// ========================================

bool AudioController::IsPlaying() const
{
    return musicVoiceInstance_ && musicVoiceInstance_->IsPlaying();
}

float AudioController::GetElapsedTime() const
{
    if (musicVoiceInstance_)
    {
        return musicVoiceInstance_->GetElapsedTime();
    }
    return 0.0f;
}

float AudioController::GetDuration() const
{
    if (musicSoundInstance_)
    {
        return musicSoundInstance_->GetDuration();
    }
    return 0.0f;
}

// ========================================
// 設定
// ========================================

void AudioController::SetVolume(float _volume)
{
    volume_ = _volume;
    if (musicVoiceInstance_)
    {
        musicVoiceInstance_->SetVolume(_volume);
    }
}

void AudioController::SetPlaySpeed(float _speed)
{
    playSpeed_ = _speed;
    if (musicVoiceInstance_)
    {
        musicVoiceInstance_->SetPlaySpeed(_speed);
    }
}

void AudioController::PlayForBPMSet()
{
    if (!musicSoundInstance_)
    {
        Debug::Log("No audio loaded for BPM setting\n");
        return;
    }

    if (voiceInstanceForBPMSet_ && voiceInstanceForBPMSet_->IsPlaying())
        return;

    // 既存の再生を停止
    Stop();
    StopForBPMSet();
    // 最初から再生（ループ再生）
    voiceInstanceForBPMSet_ = musicSoundInstance_->Play(volume_, true);
    if (voiceInstanceForBPMSet_)
    {
        voiceInstanceForBPMSet_->SetPlaySpeed(playSpeed_);
    }
}

void AudioController::StopForBPMSet()
{
    if (voiceInstanceForBPMSet_)
    {
        voiceInstanceForBPMSet_->Stop();
        voiceInstanceForBPMSet_ = nullptr;
    }
}

} // namespace BME
