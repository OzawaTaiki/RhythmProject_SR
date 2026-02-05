#include "LaneEdgeEffect.h"

#include <Application/Lane/Lane.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>

namespace
{
Vector3 leftLaneEdgePos = Vector3(-Lane::GetTotalWidth() / 2.0f, 0.0f, 0.0f);
Vector3 rightLaneEdgePos = Vector3(Lane::GetTotalWidth() / 2.0f, 0.0f, 0.0f);
float laneLength = 100.0f;
uint32_t circleTexture_ = 0;
uint32_t gradationTexture_ = 0;
};

void LaneEdgeEffects::Initialize()
{
    laneLength = Lane::GetLaneLength();
    leftLaneEdgePos = Vector3(-Lane::GetTotalWidth() / 2.0f, 0.0f, 0.0f);
    rightLaneEdgePos = Vector3(Lane::GetTotalWidth() / 2.0f, 0.0f, 0.0f);

    jsonbinder_ = std::make_unique<JsonBinder>("laneEdgeEffect", "Resources/Data/Effect/");

    circleTexture_= TextureManager::GetInstance()->Load("circle.png");
    gradationTexture_ = TextureManager::GetInstance()->Load("gradation_01.png");

    laneEdgeParticleEffect_ = std::make_unique<LaneEdgeParticleEffect>();
    laneEdgeParticleEffect_->Initialize(jsonbinder_.get());

    laneEdgePillarEffect_ = std::make_unique<LaneEdgePillarEffect>();
    laneEdgePillarEffect_->Initialize(jsonbinder_.get());
}

void LaneEdgeEffects::Update(float deltaTime)
{
#ifdef _DEBUG
    ImGui::Begin("Lane Edge Effects Debug");
    if(ImGui::Button("Emit"))
    {
        Emit();
    }
    ImGui::SameLine();
    if (ImGui::Button("save"))
    {
        jsonbinder_->Save();
    }
#endif
    laneEdgeParticleEffect_->Update(deltaTime);
    laneEdgePillarEffect_->Update(deltaTime);

#ifdef _DEBUG
    ImGui::End();
#endif
}

void LaneEdgeEffects::Emit()
{
    laneEdgeParticleEffect_->Emit();
    laneEdgePillarEffect_->Emit();
}


/// =======================================
/// LaneEdgePillarEffect
/// =======================================
void LaneEdgePillarEffect::Initialize(JsonBinder* binder)
{
    pillarLength_ = laneLength;

    pillarParam_.color = Vector4(0.5f, 0.8f, 1.0f, 0.3f); // 青系
    pillarParam_.direction = Vector3::zero;
    pillarParam_.isBillboard = { false,false,false };
    pillarParam_.isInfiniteLife = false;
    pillarParam_.lifeTime = 0.5f;
    //pillarParam_.position = Vector3::zero; // 生成時に設定する
    pillarParam_.rotate = Vector3::zero;
    pillarParam_.rotationSpeed = Vector3::zero;
    pillarParam_.size = Vector3(1.0f, 1.0f, pillarLength_);
    pillarParam_.speed = 0.0f;

    binder->RegisterVariable("pillarParams", &pillarParam_);

}

void LaneEdgePillarEffect::Update(float deltaTime)
{
#ifdef _DEBUG
    ImGui::PushID(this);
    ImGui::SeparatorText("Lane Edge Pillar Effect");
    ImGui::DragFloat("Pillar Length", &pillarLength_, 0.1f);

    ImGui::ColorEdit4("Color", &pillarParam_.color.x);
    ImGui::DragFloat("Life Time", &pillarParam_.lifeTime, 0.01f);
    ImGui::DragFloat3("Rotate", &pillarParam_.rotate.x, 0.01f);
    ImGui::SliderAngle("rot.x", &pillarParam_.rotate.x);
    ImGui::SliderAngle("rot.y", &pillarParam_.rotate.y);
    ImGui::SliderAngle("rot.z", &pillarParam_.rotate.z);
    ImGui::DragFloat3("Size", &pillarParam_.size.x, 0.01f);
    ImGui::PopID();

#endif
    deltaTime;
    //float current = 0.0f;
    //for (auto& pillar : pillars_)
    //{
    //    current = pillar->GetCurrentTime();
    //    float progress = current / pillar->GetLifeTime();
    //    {// フェードアウト
    //        Vector4 color = pillar->GetColor();
    //        float t = Easing::EaseInOutExpo(progress);
    //        color.w = Lerp(pillarParam_.color.w, 0.0f, t);
    //    }
    //}
}

void LaneEdgePillarEffect::Emit()
{
    ParticleRenderSettings settings;
    settings.blendMode = PSOFlags::BlendMode::Add;
    settings.cullBack = false;
    ParticleInitParam originalParam = pillarParam_;
    const Vector3 offset = Vector3(-0.25f, originalParam.size.y * 0.35f, 0.0f);

    std::vector<std::unique_ptr<Particle>> tempPillars;

    pillars_.clear();
    {// 左側の柱
        auto leftPillar = std::make_unique<Particle>();
        originalParam.position = leftLaneEdgePos;
        originalParam.position.x += offset.x;
        originalParam.position.y += offset.y;
        originalParam.position.z += pillarLength_ / 2.0f; // 中心にくるように調整
        leftPillar->Initialize(originalParam);
        pillars_.push_back(leftPillar.get());
        tempPillars.push_back(std::move(leftPillar));
    }
    {// 右側の柱
        auto rightPillar = std::make_unique<Particle>();
        originalParam.position = rightLaneEdgePos ;
        originalParam.position.x -= offset.x;
        originalParam.position.y += offset.y;
        originalParam.position.z += pillarLength_ / 2.0f; // 中心にくるように調整
        originalParam.rotate.x *= -1.0f;
        rightPillar->Initialize(originalParam);
        pillars_.push_back(rightPillar.get());
        tempPillars.push_back(std::move(rightPillar));

    }
    ParticleSystem::GetInstance()->AddParticles("LaneEdge_Pillars", "Ppx_1x1_plane",
                                                std::move(tempPillars), settings, gradationTexture_, { "LaneEdgePillarModifier" });
}

/// =======================================
/// LaneEdgeParticleEffect
/// =======================================

void LaneEdgeParticleEffect::Initialize(JsonBinder* binder)
{
    particleParam_.acceleration = Vector3(0.0f, -6.0f, 0.0f);
    particleParam_.color = Vector4(0.5f, 0.8f, 1.0f, 0.5f); // 青系
    particleParam_.direction = Vector3(-1.0f, 1.0f, 0.0f);
    particleParam_.isBillboard = { true,true,true };
    particleParam_.isInfiniteLife = false;
    particleParam_.lifeTime = 0.5f;
    //particleParam_.position = Vector3::zero; // 生成時に設定する
    particleParam_.rotate = Vector3::zero;
    particleParam_.rotationSpeed = Vector3::zero;
    particleParam_.size = Vector3(0.5f, 0.5f, 0.5f);
    particleParam_.speed = 1.0f;

    binder->RegisterVariable("particleEmitCount", &emitCount_);
    binder->RegisterVariable("particleParams", &particleParam_);

    emitIntervalZ_ = laneLength / static_cast<float>(emitCount_);
}

void LaneEdgeParticleEffect::Update(float deltaTime)
{
#ifdef _DEBUG

    ImGui::PushID(this);
    ImGui::SeparatorText("Lane Edge Particle Effect");
    ImGui::InputInt("Emit Count", &emitCount_);
    ImGui::InputFloat("Emit Interval Z", &emitIntervalZ_);

    ImGui::DragFloat3("Acceleration", &particleParam_.acceleration.x, 0.1f);
    ImGui::DragFloat3("Direction", &particleParam_.direction.x, 0.1f);
    ImGui::ColorEdit4("Color", &particleParam_.color.x);
    ImGui::DragFloat("Life Time", &particleParam_.lifeTime, 0.01f);
    ImGui::DragFloat3("Rotate", &particleParam_.rotate.x, 0.01f);
    ImGui::DragFloat3("Size", &particleParam_.size.x, 0.01f);
    ImGui::DragFloat("Speed", &particleParam_.speed, 0.01f);

    ImGui::PopID();
    emitIntervalZ_ = laneLength / static_cast<float>(emitCount_);
#endif // _DEBUG

    deltaTime;
    //float current = 0.0f;
    //for (auto& particle : particles_)
    //{
    //    current = particle->GetCurrentTime();
    //    float progress = current / particle->GetLifeTime();
    //    if (progress <= 0.2f)
    //    {
    //        float size = particle->GetScale().x;
    //        float t = Easing::EaseOutExpo(progress / 0.2f);
    //        size = Lerp(particleParam_.size.x, particleParam_.size.x * 1.1f , t);
    //        particle->SetScale(Vector3(size, size, size));
    //    }
    //    else
    //    {
    //        // フェードアウト
    //        Vector4 color = particle->GetColor();
    //        float t = Easing::EaseInOutExpo(progress);
    //        color.w =Lerp(particleParam_.color.w, 0.0f, t);

    //        float size = particle->GetScale().x;
    //        size = Lerp(particleParam_.size.x, 0.3f, t);
    //        particle->SetScale(Vector3(size, size, size));
    //    }
    //}
}

void LaneEdgeParticleEffect::Emit()
{
    Vector3 leftParticleDir = particleParam_.direction;
    Vector3 rightParticleDir = leftParticleDir;
    rightParticleDir.x *= -1.0f; // 右側は反転

    ParticleInitParam originalParam = particleParam_;
    originalParam.direction = leftParticleDir;

    ParticleRenderSettings settings;
    settings.blendMode = PSOFlags::BlendMode::Add;
    settings.cullBack = false;
    std::vector<std::unique_ptr<Particle>> tempParticles;
    for (int32_t i = 0; i < emitCount_; ++i)
    {
        {// 左側のパーティクル
            auto leftParticle = std::make_unique<Particle>();
            originalParam.position = leftLaneEdgePos;
            originalParam.position.z += emitIntervalZ_ * static_cast<float>(i);
            originalParam.direction = leftParticleDir;
            leftParticle->Initialize(originalParam);
            particles_.push_back(leftParticle.get());
            tempParticles.push_back(std::move(leftParticle));
        }
        {// 右側のパーティクル
            auto rightParticle = std::make_unique<Particle>();
            originalParam.position = rightLaneEdgePos;
            originalParam.position.z += emitIntervalZ_ * static_cast<float>(i);
            originalParam.direction = rightParticleDir;
            rightParticle->Initialize(originalParam);
            particles_.push_back(rightParticle.get());
            tempParticles.push_back(std::move(rightParticle));
        }
    }
    ParticleSystem::GetInstance()->AddParticles("LaneEdge_Particles", "pZ1x1Plane",
                                                std::move(tempParticles), settings, circleTexture_, {"LaneEdgeParticleModifier"});
}

void LaneEdgeParticleModifier::Apply(Particle* particle, float deltaTime)
{
    float current = particle->GetCurrentTime();
    float progress = current / particle->GetLifeTime();
    if (progress <= 0.2f)
    {
        float size = particle->GetScale().x;
        float t = Easing::EaseOutExpo(progress / 0.2f);
        size = Lerp(size, size * 1.1f, t);
        particle->SetScale(Vector3(size, size, size));
    }
    else
    {
        // フェードアウト
        Vector4 color = particle->GetColor();
        float t = Easing::EaseInOutExpo(progress);
        color.w = Lerp(color.w, 0.0f, t);

        float size = particle->GetScale().x;
        size = Lerp(size, 0.3f, t);
        particle->SetScale(Vector3(size, size, size));
    }


    deltaTime;
}

void LaneEdgePillarModifier::Apply(Particle* particle, float deltaTime)
{
    float current = particle->GetCurrentTime();
    float progress = current / particle->GetLifeTime();
    {// フェードアウト
        Vector4 color = particle->GetColor();
        float t = Easing::EaseInOutExpo(progress);
        color.w = Lerp(color.w, 0.0f, t);
        particle->SetColor(color);
    }

    deltaTime;
}
