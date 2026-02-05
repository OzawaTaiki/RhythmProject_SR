#include "TriggerEffects.h"

#include <Math/Random/RandomGenerator.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Features/Model/Model.h>
#include <Features/Model/Primitive/Plane.h>
#include <Features/Model/Primitive/Triangle.h>
#include <Features/Effect/Manager/ParticleSystem.h>

#include <numbers>

uint32_t TriggerEffects::countPerEmit_ = 16; // 一度に発生するパーティクルの数
float TriggerEffects::baseSize_ = 0.3f;
float TriggerEffects::centerSize_ = 1.0f;
uint32_t TriggerEffects::textureHandle_ = 0;
uint32_t TriggerEffects::gradationTexture_ = 0; // グラデーション用のテクスチャ
Vector4 TriggerEffects::commonColor_ = Vector4(0.3f, 0.6f, 1.0f, 0.5f); // 共通の色

ParticleEmitter TriggerEffects::cubePop_; // ほそ長いのを出すエミッター
ParticleEmitter TriggerEffects::risingParticles_;
ParticleEmitter TriggerEffects::triangleEmitter_; // 三角形を出すエミッター
ParticleEmitter TriggerEffects::lightPillarEmitter_; // 光の柱を出すエミッター

void TriggerEffects::Initialize()
{
    // テクスチャ読み込み
    textureHandle_ = TextureManager::GetInstance()->Load("circle.png");
    gradationTexture_ = TextureManager::GetInstance()->Load("gradation.png");

    // エミッター初期化
    cubePop_.Initialize("TapEffect_01");
    triangleEmitter_.Initialize("TapEffect_Triangle");
    risingParticles_.Initialize("tap_risingParticles");
    lightPillarEmitter_.Initialize("tap_light_pillar");
}

// TODO : マジックナンバー多すぎ
void TriggerEffects::EmitCenterCircles(const Vector3& pos)
{
    ParticleRenderSettings settings;
    settings.blendMode = PSOFlags::BlendMode::Add;
    settings.cullBack = false;

    {// 中心のやつ
        std::vector<std::unique_ptr<Particle>> circleParticle;
        ParticleInitParam param;

        param.lifeTime = 0.2f;
        param.position = pos;
        param.position.y += 0.01f;
        param.size = Vector3(centerSize_, centerSize_ * 0.4f, centerSize_) * 1.3f * 0.5f;
        param.color = commonColor_;
        //param.rotate.x = std::numbers::pi_v<float> / 6.0f;
        param.speed = 0.0f;
        param.isBillboard = { false,false ,true};

        auto particle = std::make_unique<Particle>();
        particle->Initialize(param);
        circleParticle.push_back(std::move(particle));

        param.size = Vector3(centerSize_, centerSize_ * 0.4f, centerSize_)*0.5f;
        param.color = Vector4(0.6f, 0.8f, 1.0f, 0.3f);

        auto particle2 = std::make_unique<Particle>();
        particle2->Initialize(param);
        circleParticle.push_back(std::move(particle2));

        ParticleSystem::GetInstance()->AddParticles("Hit_circle", "i0o1_PlanarRing",
                                                    std::move(circleParticle), settings, textureHandle_, { "HitCircleParticleModifier" });
    }
}

void TriggerEffects::EmitSurroundingParticles(const Vector3& pos, int32_t comboLevel)
{
    // 座標設定
    const Vector3 offset = { 0.0f, 1.0f, 0.0f };

    lightPillarEmitter_.SetPosition(pos);
    lightPillarEmitter_.GenerateParticles();

    triangleEmitter_.SetPosition(pos);
    triangleEmitter_.GenerateParticles();

    if (comboLevel >= 1)
    {
        cubePop_.SetPosition(pos + offset);
        cubePop_.GenerateParticles();
    }

    if (comboLevel >= 2)
    {
        risingParticles_.SetPosition(pos);
        risingParticles_.GenerateParticles();
    }
}
