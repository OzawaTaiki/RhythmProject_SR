#pragma once

#include <Features/Effect/Emitter/ParticleEmitter.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>

#include <cstdint>

// ホールドエフェクトクラス
/// <summary>
/// ホールドノート中に再生するエフェクト（サウンドやパーティクル）を管理するクラス。
/// </summary>
class NoteHoldEffect
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    NoteHoldEffect() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~NoteHoldEffect() = default;

    /// <summary>
    /// エフェクトの初期化処理。
    /// </summary>
    void Initialize();

    /// <summary>
    /// ホールドエフェクトを再生する。
    /// </summary>
    /// <param name="_laneIndex">対象レーンのインデックス</param>
    void Play(int32_t _laneIndex);

private:

    std::shared_ptr<SoundInstance> soundInstance_; // ホールド中のサウンド
    std::shared_ptr<VoiceInstance> voiceInstance_; // 音声インスタンス

    // TODO : エミッタ名要確認
    ParticleEmitter emitter_; // パーティクルエミッタ
    ParticleEmitter emitter2_; // パーティクルエミッタ
    ParticleEmitter risingparticlesEmitter_;


};