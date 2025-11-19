#include "SpeakerEffectModifier.h"


void SpeakerRingModifier::Apply(Particle* particle, [[maybe_unused]] float deltaTime)
{
    if (particle == nullptr)
        return;

    // 拡大処理
    float progress = particle->GetCurrentTime() / particle->GetLifeTime() * 2.0f;
    float useProgress = progress;
    if (progress > 1.0f)
        useProgress = 2.0f - progress;

    const float kMinSize = 0.3f;
    const float kMaxSize = 1.5f;

    float easedProgressForSize = Easing::EaseOutExpo(useProgress);
    float easedSize = Lerp(kMinSize,kMaxSize , easedProgressForSize);

    if (progress <= 1.0f)
    {
        Vector3 size = Vector3(easedSize, easedSize, easedSize);
        particle->SetScale(size);
    }

    // フェード処理
    Vector4 color = particle->GetColor();

    // 生成時のalpah
    const float initialAlpha = 0.7f;
    color.w = initialAlpha * useProgress;

    particle->SetColor(color);


}

void SpeakerParticleModifier::Apply(Particle* particle, [[maybe_unused]] float deltaTime)
{
    if (particle == nullptr)
        return;

    // 縮小処理
    float progress = particle->GetCurrentTime() / particle->GetLifeTime();

    Vector3 size = particle->GetScale();

    const float kMinSize = 0.001f;

    float easedProgressForSize = Easing::EaseInExpo(progress);
    float easedSize = Lerp(size.x, kMinSize, easedProgressForSize);
    size = Vector3(easedSize, easedSize, easedSize);

    particle->SetScale(size);

    // フェード処理
    Vector4 color = particle->GetColor();

    progress = Easing::EaseInQuint(progress);
    // 生成時のalpah
    const float initialAlpha = 0.7f;
    color.w = initialAlpha * (1.0f - progress);

    particle->SetColor(color);

}
