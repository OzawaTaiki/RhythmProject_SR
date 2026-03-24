#pragma once

#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <System/Audio/AudioEffect.h>
#include <System/Audio/AudioEffectManager.h>

/// <summary>
/// 音楽再生用クラス。
/// </summary>
class GameMusic
{
    // voiceInstanceのラッパーのようなもの
public:

    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="musicFilePath">読み込む音楽ファイルのパス</param>
    GameMusic(const std::string& musicFilePath);

    ~GameMusic();

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <param name="rewindTime">巻き戻し時間（秒）</param>
    void Initialize(float rewindTime = 1.0f);

    /// <summary>
    /// 毎フレームの更新処理を行う
    /// </summary>
    /// <param name="deltaTime"></param>
    void Update(float deltaTime);

    /// <summary>
    /// 再生経過時間を取得する。
    /// </summary>
    float GetElapsedTime() const;

    /// <summary>
    /// 音楽を再生する。
    /// </summary>
    /// <param name="volume">再生音量</param>
    void Play(float volume);

    /// <summary>
    /// 音楽を再開する。
    /// </summary>
    void Resume();

    /// <summary>
    /// 巻き戻し付きで再開する。
    /// </summary>
    /// <param name="volume">再生音量</param>
    void ResumeWithRewind(float volume);

    /// <summary>
    /// 音楽を一時停止する。
    /// </summary>
    void Pause();

    /// <summary>
    /// 音量を設定する。
    /// </summary>
    void SetVolume(float volume);

    /// <summary>
    /// 音楽が再生中かどうかを返す。
    /// </summary>
    bool IsPlaying() const { return isMusicPlaying_; }

    /// <summary>
    /// 音楽が終了したかを返す。
    /// </summary>
    bool IsMusicEnd() const { return isMusicEnd_; }

    /// <summary>
    /// 音楽の長さを取得する。
    /// </summary>
    float GetDuration()const;

    std::shared_ptr<Engine::VoiceInstance> GetVoiceInstance();

    std::shared_ptr<Engine::SoundInstance> GetSoundInstance();

    void MusicEnd() { isMusicEnd_ = true; };

    // ダッキングをトリガーする
    void TriggerDucking(float targetVolume, float duration);

    void SetBitCrush(float bitDepth, float sampleRateReduction);

    void EnableBitCrush();

    void DisableBitCrush();

private:
    // ダッキングの状態を更新する
    void UpdateDucking(float deltaTime);

    void GenerateVoiceWithBitCrusher(float volume, float startTime);
        

private:

    std::shared_ptr<Engine::SoundInstance> soundInstance_; // 音楽データ
    std::shared_ptr<Engine::VoiceInstance> voiceInstance_; // 音楽再生インスタンス

    std::unique_ptr<Engine::VoiceCallBack> voiceCallBack_; // 音楽再生コールバック

    bool isMusicPlaying_ = false; // 音楽が再生中かどうか
    bool isMusicEnd_ = false; // 音楽が終了したかどうか
    // 巻き戻し時間
    float rewindTime_ = 1.0f;

    // 曲を停止時点での再生経過時間
    float pausedAtTime_ = 0.0f;

    struct DuckingInfo
    {
        bool isDucking = false; // ダッキング中かどうか
        float targetVolume = 1.0f; // ダッキングの目標音量
        float duckingDuration = 0.5f; // ダッキングの持続時間
        float duckingElapsed = 0.0f; // ダッキングの経過時間
        constexpr static float kNormalVolume = 1.0f; // 通常音量
    }duckingInfo_;

    Engine::AudioEffectChain effectChain_; // 音声エフェクトチェーン（Enable/Disable に使用）
};