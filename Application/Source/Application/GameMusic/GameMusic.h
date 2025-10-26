#pragma once

#include <System/Audio/SoundInstance.h>

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
    /// <param name="_musicFilePath">読み込む音楽ファイルのパス</param>
    GameMusic(const std::string& _musicFilePath);

    ~GameMusic();

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <param name="_rewindTime">巻き戻し時間（秒）</param>
    void Initialize(float _rewindTime = 1.0f);

    /// <summary>
    /// 再生経過時間を取得する。
    /// </summary>
    float GetElapsedTime() const;

    /// <summary>
    /// 音楽を再生する。
    /// </summary>
    /// <param name="_volume">再生音量</param>
    void Play(float _volume);

    /// <summary>
    /// 音楽を再開する。
    /// </summary>
    void Resume();

    /// <summary>
    /// 巻き戻し付きで再開する。
    /// </summary>
    /// <param name="_volume">再生音量</param>
    void ResumeWithRewind(float _volume);

    /// <summary>
    /// 音楽を一時停止する。
    /// </summary>
    void Pause();

    /// <summary>
    /// 音量を設定する。
    /// </summary>
    void SetVolume(float _volume);

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

    std::shared_ptr<VoiceInstance> GetVoiceInstance();

    std::shared_ptr<SoundInstance> GetSoundInstance();
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