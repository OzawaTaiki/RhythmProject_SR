#pragma once

#include <Features/Effect/Modifier/ParticleModifier.h>

// ヒットエフェクト用パーティクルモディファイア
/// <summary>
/// ヒット時に出るパーティクルを制御する修飾子クラス。
/// </summary>
class HitParticleModifier : public Engine::ParticleModifier
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    HitParticleModifier() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~HitParticleModifier() = default;

    /// <summary>
    /// 初期化処理（必要に応じてオーバーライド）。
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// パーティクルに対して毎フレーム適用される処理。
    /// </summary>
    /// <param name="particle">対象パーティクル</param>
    /// <param name="deltaTime">経過時間（秒）</param>
    void Apply(Engine::Particle* particle, float deltaTime) override;
};

/// <summary>
/// ヒットサークル表現用のパーティクル修飾子クラス。
/// </summary>
class HitCircleParticleModifier : public Engine::ParticleModifier
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    HitCircleParticleModifier() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~HitCircleParticleModifier() = default;

    /// <summary>
    /// 初期化処理。
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// パーティクルへの適用処理。
    /// </summary>
    /// <param name="particle">対象パーティクル</param>
    /// <param name="deltaTime">経過時間（秒）</param>
    void Apply(Engine::Particle* particle, float deltaTime) override;

};