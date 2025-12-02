#pragma once

#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <memory>
#include <string>

class AudioSystem;

namespace BME
{

/// <summary>
/// エディターの音声制御クラス
/// </summary>
class AudioController
{
public:
    AudioController();
    ~AudioController() = default;

    /// <summary>
    /// 音声ファイルをロード
    /// </summary>
    /// <param name="_filePath">音声ファイルパス</param>
    /// <returns>ロード成功ならtrue</returns>
    bool Load(const std::string& _filePath);

    /// <summary>
    /// 指定時間から再生
    /// </summary>
    /// <param name="_startTime">開始時間（秒）</param>
    void Play(float _startTime = 0.0f);

    /// <summary>
    /// 停止
    /// </summary>
    void Stop();

    /// <summary>
    /// 最初から再生
    /// </summary>
    void Restart();

    /// <summary>
    /// 再生中か確認
    /// </summary>
    bool IsPlaying() const;

    /// <summary>
    /// 経過時間を取得
    /// </summary>
    float GetElapsedTime() const;

    /// <summary>
    /// 総再生時間を取得
    /// </summary>
    float GetDuration() const;

    /// <summary>
    /// 音量を設定（0.0~1.0）
    /// </summary>
    void SetVolume(float _volume);

    /// <summary>
    /// 再生速度を設定
    /// </summary>
    void SetPlaySpeed(float _speed);

    /// <summary>
    /// 音声データが存在するか確認
    /// </summary>
    bool HasAudio() const { return musicSoundInstance_ != nullptr; }

    /// <summary>
    /// SoundInstanceを取得（Waveform表示など外部で必要な場合）
    /// </summary>
    SoundInstance* GetSoundInstance() const { return musicSoundInstance_.get(); }

    /// <summary>
    /// VoiceInstanceを取得（BeatManager連携など外部で必要な場合）
    /// </summary>
    std::shared_ptr<VoiceInstance> GetVoiceInstance() const { return musicVoiceInstance_; }

    void PlayForBPMSet();

    void StopForBPMSet();
private:
    AudioSystem* audioSystem_ = nullptr;                        // AudioSystemのポインタ
    std::shared_ptr<SoundInstance> musicSoundInstance_;         // 音声インスタンス
    std::shared_ptr<VoiceInstance> musicVoiceInstance_;         // 再生中の音声インスタンス
    float volume_ = 1.0f;                                       // 音量
    float playSpeed_ = 1.0f;                                    // 再生速度

    std::shared_ptr<VoiceInstance> voiceInstanceForBPMSet_; // BPM設定用の音声インスタンス
};

} // namespace BME
