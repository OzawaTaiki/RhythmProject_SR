#pragma once

#include <Features/Effect/Emitter/ParticleEmitter.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>

#include <cstdint>

class NoteHoldEffect
{
public:
    // コンストラクタ
    NoteHoldEffect() = default;
    // デストラクタ
    ~NoteHoldEffect() = default;

    void Initialize();



    void Play(int32_t _laneIndex);

private:

    std::shared_ptr<SoundInstance> soundInstance_; // サウンドインスタンス
    std::shared_ptr<VoiceInstance> voiceInstance_; // 音声インスタンス

    ParticleEmitter emitter_; // パーティクルエミッター
    ParticleEmitter emitter2_; // パーティクルエミッター
};