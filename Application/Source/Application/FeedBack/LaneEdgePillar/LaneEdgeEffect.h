#pragma once

#include <Features/Json/JsonBinder.h>
#include <Features/Effect/Particle/Particle.h>

#include <memory>

class LaneEdgePillarEffect;
class LaneEdgeParticleEffect;

/// <summary>
/// レーン端エフェクト全体を束ねるファサードクラス。
/// LaneEdgePillarEffect（柱状光）と LaneEdgeParticleEffect（パーティクル）を統括して管理する。
/// </summary>
class LaneEdgeEffects
{
public:
    LaneEdgeEffects() = default;
    ~LaneEdgeEffects() = default;

    void Initialize();
    void Update(float deltaTime);
    void Emit();

private:

    std::unique_ptr<LaneEdgeParticleEffect> laneEdgeParticleEffect_;
    std::unique_ptr<LaneEdgePillarEffect> laneEdgePillarEffect_;
    std::unique_ptr<Engine::JsonBinder> jsonbinder_;
};

/// <summary>
/// レーン端に縦方向の柱状光エフェクトを表示するクラス。
/// パーティクルをピラー（柱）として管理し、Emit() で発生させる。
/// </summary>
class LaneEdgePillarEffect
{
public:
    LaneEdgePillarEffect() = default;
    ~LaneEdgePillarEffect() = default;

    void Initialize(Engine::JsonBinder* binder);
    void Update(float deltaTime);
    void Emit();
private:

    std::vector<Engine::Particle*> pillars_;
    // 管理権はマネージャーにあるから生ポ
    Engine::ParticleInitParam pillarParam_;
    float pillarLength_ = 50.0f;
};

/// <summary>
/// レーン端にパーティクルを散らすエフェクトクラス。
/// Z方向に一定間隔でパーティクルを Emit() し、レーン端の演出を担う。
/// </summary>
class LaneEdgeParticleEffect
{
public:
    LaneEdgeParticleEffect() = default;
    ~LaneEdgeParticleEffect() = default;

    void Initialize(Engine::JsonBinder* binder);
    void Update(float deltaTime);
    void Emit();
private:
    std::vector<Engine::Particle*> particles_;
    //  管理権はマネージャーにあるから生ポ
    Engine::ParticleInitParam particleParam_;
    int32_t emitCount_ = 10;// 出す数 ここから座標間隔等計算する
    // 発生間隔座標
    float emitIntervalZ_ = 5.0f;
};

#include <Features/Effect/Modifier/ParticleModifier.h>
class LaneEdgeParticleModifier : public Engine::ParticleModifier
{
public:
    LaneEdgeParticleModifier() = default;
    ~LaneEdgeParticleModifier() = default;

    void Apply(Engine::Particle* particle, float deltaTime) override;
};

class LaneEdgePillarModifier : public Engine::ParticleModifier
{
public:
    LaneEdgePillarModifier() = default;
    ~LaneEdgePillarModifier() = default;

    void Apply(Engine::Particle* particle, float deltaTime) override;
};