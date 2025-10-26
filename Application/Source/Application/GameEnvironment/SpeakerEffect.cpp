#include "SpeakerEffect.h"

#include <Features/Model/ObjectModel.h>

SpeakerEffect::SpeakerEffect()
{
    // 各種エミッター初期化
    ringEmitter_.Initialize("SpeakerRing");
    rectanglleParticleEmitter_.Initialize("Speaker_particle");
    triangleParticleEmitter_.Initialize("Sepaker_Triangle");
}

void SpeakerEffect::PlaySpeakerEffect(const Vector3& _pos)
{
    // 座標の設定
    ringEmitter_.SetPosition(_pos);
    triangleParticleEmitter_.SetPosition(_pos);
    rectanglleParticleEmitter_.SetPosition(_pos);

    // 生成
    ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectanglleParticleEmitter_.GenerateParticles();
}

void SpeakerEffect::PlaySpeakerEffect(ObjectModel* _parent)
{
    // 親オブジェクトのワールド変換行列設定
    auto worldTransform = _parent->GetWorldTransform();
    ringEmitter_.SetParentTransform(worldTransform);
    triangleParticleEmitter_.SetParentTransform(worldTransform);
    rectanglleParticleEmitter_.SetParentTransform(worldTransform);

    // 生成
    ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectanglleParticleEmitter_.GenerateParticles();
}
