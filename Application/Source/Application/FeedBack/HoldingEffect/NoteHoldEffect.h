#pragma once

#include <Features/Effect/Emitter/ParticleEmitter.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>

#include <cstdint>

// ホールドエフェクトクラス
class NoteHoldEffect
{
public:
    NoteHoldEffect() = default;
    ~NoteHoldEffect() = default;

    // 初期化
    void Initialize();

    // 再生
    void Play(int32_t _laneIndex);

private:

    std::shared_ptr<SoundInstance> soundInstance_; // ホールド中のサウンド
    std::shared_ptr<VoiceInstance> voiceInstance_; // 音声インスタンス

    // TODO : エミッター名謎
    ParticleEmitter emitter_; // パーティクルエミッター
    ParticleEmitter emitter2_; // パーティクルエミッター
};