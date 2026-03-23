#pragma once

#include <System/Audio/SoundEngine.h>

#include <memory>
#include <vector>
#include <string>

/// <summary>
/// ジャッジ時の効果音を管理するクラス。
/// SoundEngine 経由で SE を再生する。
/// </summary>
class JudgeSound
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    JudgeSound() = default;

    /// <summary>
    /// デストラクタ（再生中のリソースを解放する）
    /// </summary>
    ~JudgeSound();

    /// <summary>
    /// 初期化処理。
    /// SoundEngine::LoadSoundData() が先に呼ばれている前提。
    /// </summary>
    /// <param name="volume">初期音量（0.0 - 1.0）</param>
    void Initialize(float volume = 0.5f);

    /// <summary>
    /// 効果音を再生する。
    /// </summary>
    void Play();

    /// <summary>
    /// 再生が終了した音声インスタンスをクリーンアップする。
    /// </summary>
    void CleanupStoppedVoices();

    /// <summary>
    /// 全ての効果音を停止する。
    /// </summary>
    void StopAllSounds();


    // ゲッター・セッター

    /// <summary>
    /// 音量を設定する。
    /// </summary>
    void SetVolume(float volume) { volume_ = volume; }

    /// <summary>
    /// 現在の音量を取得する。
    /// </summary>
    float GetVolume() const { return volume_; }


private: // メンバ変数

    float volume_ = 0.5f;

    // 再生中のハンドル一覧（VoiceInstance の代わり）
    std::vector<Engine::SoundHandle> handles_;
};
