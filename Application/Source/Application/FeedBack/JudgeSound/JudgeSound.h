#pragma once

#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>


#include <memory>
#include <vector>
#include <string>

// 判定時にサウンドを鳴らすためのクラス
/// <summary>
/// ジャッジ時の効果音を管理するクラス。
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
    /// </summary>
    /// <param name="_volume">初期音量（0.0 - 1.0）</param>
    /// <param name="_soundFilePath">SEファイルパス（省略時はデフォルトパス）</param>
    void Initialize(float _volume = 0.5f , const std::string& _soundFilePath = "Resources/Sounds/SE/JudgeSound.wav");

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
    void SetVolume(float _volume) { volume_ = _volume; }

    /// <summary>
    /// 現在の音量を取得する。
    /// </summary>
    float GetVolume() const { return volume_; }


private: // メンバ変数

    std::shared_ptr<SoundInstance> soundInstance_; // 音声インスタンス
    std::vector<std::shared_ptr<VoiceInstance>> voiceInstance_; // 音声の再生インスタンス

    std::string soundFilePath_; // サウンドファイルのパス


    float volume_ = 0.5f; // 音量

};