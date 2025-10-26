#pragma once

#include <Features/Effect/Modifier/ParticleModifier.h>

// ライトピラーエフェクトのパーティクル修飾子
/// <summary>
/// 光の柱表現を行うパーティクル修飾子クラス。
/// </summary>
class LightPillarModifier : public ParticleModifier
{
public:

    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    LightPillarModifier() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~LightPillarModifier() override = default;

    /// <summary>
    /// パーティクルに対して毎フレーム適用される処理。
    /// </summary>
    /// <param name="_particle">対象パーティクル</param>
    /// <param name="_deltaTime">経過時間（秒）</param>
    void Apply(Particle* _particle, float _deltaTime) override;
};