#include "SpeakerEffectModifier.h"


void SpeakerRingModifier::Apply(Particle* _particle, [[maybe_unused]] float _deltaTime)
{
    if (_particle == nullptr)
        return;

    // 拡大処理
    float progress = _particle->GetCurrentTime() / _particle->GetLifeTime() * 2.0f;
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
        _particle->SetScale(size);
    }

    // フェード処理
    Vector4 color = _particle->GetColor();

    // 生成時のalpah
    const float initialAlpha = 0.7f;
    color.w = initialAlpha * useProgress;

    _particle->SetColor(color);


}

void SpeakerParticleModifier::Apply(Particle* _particle, [[maybe_unused]] float _deltaTime)
{
    if (_particle == nullptr)
        return;

    // 縮小処理
    float progress = _particle->GetCurrentTime() / _particle->GetLifeTime();

    Vector3 size = _particle->GetScale();

    const float kMinSize = 0.001f;

    float easedProgressForSize = Easing::EaseInExpo(progress);
    float easedSize = Lerp(size.x, kMinSize, easedProgressForSize);
    size = Vector3(easedSize, easedSize, easedSize);

    _particle->SetScale(size);

    // フェード処理
    Vector4 color = _particle->GetColor();

    progress = Easing::EaseInQuint(progress);
    // 生成時のalpah
    const float initialAlpha = 0.7f;
    color.w = initialAlpha * (1.0f - progress);

    _particle->SetColor(color);

}
