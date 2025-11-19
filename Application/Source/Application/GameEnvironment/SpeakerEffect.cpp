#include "SpeakerEffect.h"

#include <Features/Model/ObjectModel.h>

SpeakerEffect::SpeakerEffect()
{
    // 各種エミッター初期化
    ringEmitter_.Initialize("SpeakerRing");
    rectanglleParticleEmitter_.Initialize("Speaker_particle");
    triangleParticleEmitter_.Initialize("Sepaker_Triangle");
}

void SpeakerEffect::PlaySpeakerEffect(const Vector3& pos)
{
    // 座標の設定
    ringEmitter_.SetPosition(pos);
    triangleParticleEmitter_.SetPosition(pos);
    rectanglleParticleEmitter_.SetPosition(pos);

    // 生成
    ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectanglleParticleEmitter_.GenerateParticles();
}

void SpeakerEffect::PlaySpeakerEffect(ObjectModel* parent)
{
    // 親オブジェクトのワールド変換行列設定
    auto worldTransform = parent->GetWorldTransform();
    ringEmitter_.SetParentTransform(worldTransform);
    triangleParticleEmitter_.SetParentTransform(worldTransform);
    rectanglleParticleEmitter_.SetParentTransform(worldTransform);

    ringEmitter_.SetParentTransform(parent->GetWorldTransform());
    triangleParticleEmitter_.SetParentTransform(parent->GetWorldTransform());
    rectanglleParticleEmitter_.SetParentTransform(parent->GetWorldTransform());

    //ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectanglleParticleEmitter_.GenerateParticles();
}
