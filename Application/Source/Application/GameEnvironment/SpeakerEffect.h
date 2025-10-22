#pragma once

#include <Features/Effect/Effect/Effect.h>

// 前方宣言
class ObjectModel;

// スピーカーエフェクト
class SpeakerEffect
{
public:
    SpeakerEffect();
    ~SpeakerEffect() = default;

    /// <summary>
    /// スピーカーエフェクト再生
    /// </summary>
    /// <param name="_pos">エフェクトの出現座標</param>
    void PlaySpeakerEffect(const Vector3& _pos);

    /// <summary>
    /// スピーカーエフェクト再生
    /// </summary>
    /// <param name="_parent">エフェクトの親オブジェクト</param>
    void PlaySpeakerEffect(ObjectModel* _parent);

private:

    // 各種エミッター
    ParticleEmitter ringEmitter_;
    ParticleEmitter triangleParticleEmitter_;
    ParticleEmitter rectanglleParticleEmitter_;
};