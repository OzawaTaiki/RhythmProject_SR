#pragma once

#include <System/Audio/SoundInstance.h>

// 音楽再生用クラス
class GameMusic
{
    // voiceInstanceのラッパーのようなもの
public:

    GameMusic(const std::string& _musicFilePath);
    ~GameMusic();

    // 初期化
    void Initialize(float _rewindTime = 1.0f);
    // 再生経過時間を取得
    float GetElapsedTime() const;
    // 音楽再生
    void Play(float _volume);
    // 音楽再開
    void Resume();
    // 音楽再開（巻き戻し付き）
    void ResumeWithRewind(float _volume);
    // 音楽一時停止
    void Pause();
    // 音量設定
    void SetVolume(float _volume);
    // 音楽が再生中かどうか
    bool IsPlaying() const { return isMusicPlaying_; }
    // 音楽が終了したかどうか
    bool IsMusicEnd() const { return isMusicEnd_; }
    // 音楽の長さを取得
    float GetDuration()const;
private:

    void MusicEnd() { isMusicEnd_ = true; };

private:

    std::shared_ptr<SoundInstance> soundInstance_; // 音楽データ
    std::shared_ptr<VoiceInstance> voiceInstance_; // 音楽再生インスタンス

    std::unique_ptr<VoiceCallBack> voiceCallBack_; // 音楽再生コールバック

    bool isMusicPlaying_ = false; // 音楽が再生中かどうか
    bool isMusicEnd_ = false; // 音楽が終了したかどうか
    // 巻き戻し時間
    float rewindTime_ = 1.0f;

    // 曲を停止時点での再生経過時間
    float pausedAtTime_ = 0.0f;
};