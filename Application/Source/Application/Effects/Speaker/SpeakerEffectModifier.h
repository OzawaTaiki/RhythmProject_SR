#pragma once

#include <Features/Effect/Modifier/ParticleModifier.h>

// スピーカーエフェクト用のパーティクルモディファイア
/// <summary>
/// スピーカーのリング表現用パーティクル修飾子。
/// </summary>
class SpeakerRingModifier : public ParticleModifier
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    SpeakerRingModifier() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SpeakerRingModifier() = default;

    /// <summary>
    /// パーティクルに対する適用処理（毎フレーム呼ばれる）。
    /// </summary>
    /// <param name="_particle">対象のパーティクル</param>
    /// <param name="_deltaTime">前フレームからの経過時間（秒）</param>
    void Apply(Particle* _particle, float _deltaTime) override;
};

/// <summary>
/// スピーカー周辺のパーティクル表現を操作する修飾子。
/// </summary>
class SpeakerParticleModifier : public ParticleModifier
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    SpeakerParticleModifier() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SpeakerParticleModifier() = default;

    /// <summary>
    /// パーティクルへの適用処理。
    /// </summary>
    /// <param name="_particle">対象のパーティクル</param>
    /// <param name="_deltaTime">経過時間（秒）</param>
    void Apply(Particle* _particle, float _deltaTime) override;
};