#pragma once

#include <System/Audio/SoundInstance.h>

class GameMusic // voiceInstanceのラッパー
{
public:

    GameMusic(const std::string& _musicFilePath);
    ~GameMusic();

    void Initialize(float _rewindTime = 1.0f);

    float GetElapsedTime() const;

    void Play(float _volume);

    void Resume();

    void ResumeWithRewind(float _volume);

    void Pause();

    void SetVolume(float _volume);

    bool IsPlaying() const { return isMusicPlaying_; }

    bool IsMusicEnd() const { return isMusicEnd_; }

    float GetDuration()const;

    std::shared_ptr<VoiceInstance> GetVoiceInstance();
private:

    void MusicEnd() { isMusicEnd_ = true; };

private:

    std::shared_ptr<SoundInstance> soundInstance_;
    std::shared_ptr<VoiceInstance> voiceInstance_;

    std::unique_ptr<VoiceCallBack> voiceCallBack_;

    bool isMusicPlaying_ = false; // 音楽が再生中かどうか
    bool isMusicEnd_ = false; // 音楽が終了したかどうか
    // 巻き戻し時間
    float rewindTime_ = 1.0f;

    // 曲を停止時点での再生経過時間
    float pausedAtTime_ = 0.0f;
};