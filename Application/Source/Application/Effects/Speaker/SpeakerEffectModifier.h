#pragma once

#include <Features/Effect/Modifier/ParticleModifier.h>

// スピーカーエフェクト用のパーティクルモディファイア
class SpeakerRingModifier : public ParticleModifier
{
public:
    SpeakerRingModifier() = default;
    ~SpeakerRingModifier() = default;

    void Apply(Particle* _particle, float _deltaTime) override;
};

// スピーカーエフェクト用のパーティクルモディファイア
class SpeakerParticleModifier : public ParticleModifier
{
public:
    SpeakerParticleModifier() = default;
    ~SpeakerParticleModifier() = default;

    void Apply(Particle* _particle, float _deltaTime) override;
};