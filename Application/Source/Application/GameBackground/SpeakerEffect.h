#pragma once

#include <Features/Effect/Effect/Effect.h>

// 前方宣言
namespace Engine { class ObjectModel; }

// スピーカーエフェクト
/// <summary>
/// スピーカーエフェクト。
/// </summary>
class SpeakerEffect
{
public:
    SpeakerEffect();
    ~SpeakerEffect() = default;

    /// <summary>
    /// 指定座標でスピーカーエフェクトを再生する。
    /// </summary>
    /// <param name="pos">エフェクトの出現座標</param>
    void PlaySpeakerEffect(const Engine::Vector3& pos);

    /// <summary>
    /// 指定オブジェクトを親にしてスピーカーエフェクトを再生する。
    /// </summary>
    /// <param name="parent">エフェクトの親オブジェクト</param>
    void PlaySpeakerEffect(Engine::ObjectModel* parent, float delayTime);

private:
    // 各種エミッター
    Engine::ParticleEmitter ringEmitter_;
    Engine::ParticleEmitter triangleParticleEmitter_;
    Engine::ParticleEmitter rectangleParticleEmitter_;

};
