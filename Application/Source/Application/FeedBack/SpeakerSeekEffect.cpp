#include "SpeakerSeekEffect.h"

#include <Features/Effect/Manager/ParticleSystem.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Math/Vector/VectorFunction.h>
#include <Math/Easing.h>
#include <Math/Random/RandomGenerator.h>

namespace
{
Vector4 color_ = Vector4(0.417f, 0.912f, 1.000f, 0.522f);
Vector3 size = Vector3(0.15f, 0.15f, 0.3f);
float duration_ = 1.0f; // 軌道にかける時間

float maxVerticalBulge_ = -2.0f; // 縦方向の膨らみ具合
float minVerticalBulge_ = -1.0f;
float maxHorizontalBulge_ = 1.0f; // 横方向の膨らみ具合
float minHorizontalBulge_ = -1.0f;

Vector3 positoinOffset_ = Vector3(0.0f, 0.5f, 0.0f);

}

void SpeakerSeekEffect::Initialize()
{
    // テクスチャ読み込み
    textureHandle_ = TextureManager::GetInstance()->Load("circle.png");
}

void SpeakerSeekEffect::Update(float deltaTime)
{
#ifdef _DEBUG
    ImGui::Begin("Speaker Seek Effect Debug");
    ImGui::DragFloat("Duration", &duration_, 0.01f, 0.1f, 5.0f);
    ImGui::DragFloat("Min Vertical Bulge", &minVerticalBulge_, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Max Vertical Bulge", &maxVerticalBulge_, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Min Horizontal Bulge", &minHorizontalBulge_, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Max Horizontal Bulge", &maxHorizontalBulge_, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat3("Pos offset", &positoinOffset_.x, 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat3("Size", &size.x, 0.01f, 0.1f, 5.0f);
    ImGui::ColorEdit4("Color", &color_.x);
    if (ImGui::Button("Emit"))
        Emit(Vector3(-10, -4, 20), Vector3(-3, 0, 0), 10);
    ImGui::End();
#endif

    // パーティクルを楕円軌道に沿って移動
    for (size_t i = 0; i < particles_.size(); ++i)
    {
        auto particle = particles_[i];
        if (!particle || !particle->IsAlive())
            continue;

        // 時間を進める
        paths_[i].time += deltaTime;

        // 進行度を計算（0.0～1.0）
        float t = paths_[i].time / paths_[i].duration;

        if (t >= 1.0f)
        {
            // 目標位置に到達
            // 消す
            particle->SetAlive(false);
        }
        else
        {
            // 楕円軌道上の位置を計算
            Vector3 newPos = CalculateEllipticalPosition(paths_[i]);
            particle->SetPosition(newPos);
        }
    }

    // 死んだパーティクルを削除（リストから）
    for (size_t i = 0; i < particles_.size();)
    {
        if (!particles_[i] || !particles_[i]->IsAlive())
        {
            particles_.erase(particles_.begin() + i);
            paths_.erase(paths_.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

void SpeakerSeekEffect::Emit(const Vector3& startPos, const Vector3& targetPos, uint32_t count)
{
    ParticleRenderSettings settings;
    settings.blendMode = PSOFlags::BlendMode::Add;
    settings.cullBack = false;

    auto rand = RandomGenerator::GetInstance();

    for (uint32_t i = 0; i < count; ++i)
    {
        ParticleInitParam param;
        param.position = startPos + rand->GetRandValue(Vector3::zero, positoinOffset_);
        param.size = size;
        param.color = color_;
        param.lifeTime = duration_;  // 軌道時間
        param.speed = 0.0f;  // Update()で制御するので0
        param.isBillboard = { true, true, false };

        auto particle = std::make_unique<Particle>();
        particle->Initialize(param);

        // ParticleSystemに追加する前に生ポインタを保持
        Particle* rawPtr = particle.get();

        // ParticleSystemに追加（所有権を渡す）
        ParticleSystem::GetInstance()->AddParticle(
            "SpeakerSeekParticles",
            "i0o1_PlanarRing",
            std::move(particle),
            settings,
            textureHandle_,
            {}  // モディファイアは使わない
        );

        // 楕円軌道情報を設定
        EllipticalPath path;
        path.startPos = param.position;
        path.targetPos = targetPos + rand->GetRandValue(Vector3::zero, positoinOffset_);
        path.time = 0.0f;
        path.duration = duration_;
        float v = rand->GetRandValue(minVerticalBulge_, maxVerticalBulge_);
        float h = rand->GetRandValue(minHorizontalBulge_, maxHorizontalBulge_);
        path.verticalBulge = v;
        path.horizontalBulge = h;
        path.worldUp = worldUp_;

        // 生ポインタと軌道情報を記録
        particles_.push_back(rawPtr);
        paths_.push_back(path);
    }
}

float SpeakerSeekEffect::GetDuration() const
{
    return duration_;
}

void SpeakerSeekEffect::Clear()
{
    particles_.clear();
    paths_.clear();
    ParticleSystem::GetInstance()->ClearParticles("SpeakerSeekParticles");
}

Vector3 SpeakerSeekEffect::CalculateEllipticalPosition(const EllipticalPath& path) const
{
    // 進行度（0.0～1.0）
    float t = path.time / path.duration;
    if (t > 1.0f) t = 1.0f;
    t = Easing::EaseInCubic(t);

    // 始点と終点のベクトル
    Vector3 direction = path.targetPos - path.startPos;
    float distance = Length(direction);

    if (distance < 0.001f)
    {
        return path.startPos;  // 距離がほぼゼロなら始点を返す
    }

    direction = Normalize(direction);

    // 進行方向に対する右方向ベクトル
    Vector3 right = Normalize(Cross(direction, path.worldUp));

    // 上方向を再計算（右方向×進行方向）
    Vector3 up = Normalize(Cross(right, direction));

    // 楕円のパラメータ（0度から180度）
    float angle = t * 3.14159265f;  // π

    // 楕円上の座標
    float x = distance * 0.5f * cosf(angle);      // 進行方向（-0.5～0.5の範囲で動く）
    float y = path.verticalBulge * sinf(angle);   // 上方向
    float z = path.horizontalBulge * sinf(angle); // 右方向

    // 中点
    Vector3 center = path.startPos + direction * (distance * 0.5f);

    // 最終位置を計算
    Vector3 result = center + direction * x + up * y + right * z;

    return result;
}