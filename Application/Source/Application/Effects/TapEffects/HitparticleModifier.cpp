#include "HitparticleModifier.h"

using namespace Engine;

void HitParticleModifier::Initialize()
{
}

void HitParticleModifier::Apply(Particle* particle, float deltaTime)
{
    // フェードアウト
    float t = particle->GetCurrentTime() / particle->GetLifeTime();

    float easedt = Easing::Func(Easing::EasingFunc::EaseInOutCirc)(t);
    Vector4 color = particle->GetColor();
    color.w = 1.0f - easedt;

    particle->SetColor(color);

    // 減速
    float deceleration = 3.0f;
    float speed = particle->GetSpeed();
    speed -= deceleration * deltaTime;
    if (speed < 0.0f)
        speed = 0.0f;
    particle->SetSpeed(speed);
}

void HitCircleParticleModifier::Initialize()
{
}

void HitCircleParticleModifier::Apply(Particle* particle, [[maybe_unused]] float deltaTime)
{
    // フェードアウト
    float t = particle->GetCurrentTime() / particle->GetLifeTime();
    float easedt = Easing::Func(Easing::EasingFunc::EaseInQuart)(t);

    Vector4 color = particle->GetColor();
    color.w = 1.0f - easedt;

    particle->SetColor(color);

    // サイズ縮小
    float easedTSize = 1.0f - Easing::Func(Easing::EasingFunc::EaseInExpo)(t);

    Vector3 size = particle->GetScale();
    const float minSize = 0.3f;

    size.x *= easedTSize;
    size.z *= easedTSize;

    size.x = (std::max)(size.x, minSize);
    size.z = (std::max)(size.z, minSize);

    particle->SetScale(size);
}