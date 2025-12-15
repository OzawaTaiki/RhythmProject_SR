#pragma once

#include <Features/Json/JsonBinder.h>
#include <Features/Effect/Particle/Particle.h>

#include <memory>

class LaneEdgePillarEffect;
class LaneEdgeParticleEffect;

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
    std::unique_ptr<JsonBinder> jsonbinder_;
};

class LaneEdgePillarEffect
{
public:
    LaneEdgePillarEffect() = default;
    ~LaneEdgePillarEffect() = default;

    void Initialize(JsonBinder* binder);
    void Update(float deltaTime);
    void Emit();
private:

    std::vector<Particle*> pillars_;
    // 管理権はマネージャーにあるから生ポ
    ParticleInitParam pillarParam_;
    float pillarLength_ = 50.0f;
};

class LaneEdgeParticleEffect
{
public:
    LaneEdgeParticleEffect() = default;
    ~LaneEdgeParticleEffect() = default;

    void Initialize(JsonBinder* binder);
    void Update(float deltaTime);
    void Emit();
private:
    std::vector<Particle*> particles_;
    //  管理権はマネージャーにあるから生ポ
    ParticleInitParam particleParam_;
    int32_t emitCount_ = 10;// 出す数 ここから座標間隔等計算する
    // 発生間隔座標
    float emitIntervalZ_ = 5.0f;
};

#include <Features/Effect/Modifier/ParticleModifier.h>
class LaneEdgeParticleModifier : public ParticleModifier
{
public:
    LaneEdgeParticleModifier() = default;
    ~LaneEdgeParticleModifier() = default;

    void Apply(Particle* particle, float deltaTime) override;
};

class LaneEdgePillarModifier : public ParticleModifier
{
public:
    LaneEdgePillarModifier() = default;
    ~LaneEdgePillarModifier() = default;

    void Apply(Particle* particle, float deltaTime) override;
};