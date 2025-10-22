#pragma once

#include <Features/Effect/Modifier/ParticleModifier.h>

// ライトピラーエフェクトのパーティクル修飾子
class LightPillarModifier : public ParticleModifier
{
public:

    LightPillarModifier() = default;
    ~LightPillarModifier() override = default;

    void Apply(Particle* _particle, float _deltaTime) override;
};