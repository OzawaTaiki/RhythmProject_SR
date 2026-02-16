#include "SpeakerEffect.h"

#include <Features/Model/ObjectModel.h>
#include <Features/Event/EventManager.h>
#include <Application/GameEnvironment/GameEnvironment.h>


SpeakerEffect::SpeakerEffect()
{
    // 各種エミッター初期化
    ringEmitter_.Initialize("SpeakerRing");
    rectangleParticleEmitter_.Initialize("Speaker_particle");
    triangleParticleEmitter_.Initialize("Speaker_Triangle");
}

void SpeakerEffect::PlaySpeakerEffect(const Vector3& pos)
{
    // 座標の設定
    ringEmitter_.SetPosition(pos);
    triangleParticleEmitter_.SetPosition(pos);
    rectangleParticleEmitter_.SetPosition(pos);

    // 生成
    ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectangleParticleEmitter_.GenerateParticles();
}

void SpeakerEffect::PlaySpeakerEffect(ObjectModel* parent, float delayTime)
{
    // 親オブジェクトのワールド変換行列設定
    auto worldTransform = parent->GetWorldTransform();
    ringEmitter_.SetParentTransform(worldTransform);
    triangleParticleEmitter_.SetParentTransform(worldTransform);
    rectangleParticleEmitter_.SetParentTransform(worldTransform);

    ringEmitter_.SetParentTransform(parent->GetWorldTransform());
    triangleParticleEmitter_.SetParentTransform(parent->GetWorldTransform());
    rectangleParticleEmitter_.SetParentTransform(parent->GetWorldTransform());

    //ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectangleParticleEmitter_.GenerateParticles();
    ColorChangeEvent eventData;
    eventData.targets = parent;
    eventData.delayTime = delayTime;
    EventManager::GetInstance()->DispatchEvent(GameEvent("SpeakerEffectColorChange", &eventData));
}
