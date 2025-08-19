#pragma once

#include <Features/Effect/Effect/Effect.h>

class ObjectModel;
class SpeakerEffect
{
public:
    SpeakerEffect();
    ~SpeakerEffect() = default;

    void PlaySpeakerEffect(const Vector3& _pos);
    void PlaySpeakerEffect(ObjectModel* _parent);

private:

    ParticleEmitter ringEmitter_;
    ParticleEmitter triangleParticleEmitter_;
    ParticleEmitter rectanglleParticleEmitter_;
};