#include "LightPillarModifier.h"

void LightPillarModifier::Apply(Particle* particle, float deltaTime)
{
    if (particle == nullptr)
        return;

    // サイズの最大値と最小値を定義
    const Vector3 maxSize = { 0.7f, 0.7f, 0.7f };
    const Vector3 minSize = { 0.5f, 0.5f, 0.5f };

    Vector3 scale = particle->GetScale();

    float lifeTime = particle->GetLifeTime();
    float currentTime = particle->GetCurrentTime();

    float progress = currentTime / lifeTime;

    // サイズの変化を線形補間
    progress = Easing::EaseInBack(progress);

    scale = Vector3::Lerp(maxSize, minSize, progress);

    particle->SetScale(scale);
}
