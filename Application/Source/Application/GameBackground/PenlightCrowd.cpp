#include "PenlightCrowd.h"

#include <Features/Model/InstancedObjectModel.h>
#include <Features/Model/Primitive/Cylinder.h>
#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <Math/Matrix/MatrixFunction.h>
#include <Math/Color/Color.h>

#ifdef _DEBUG
#include <Debug/ImGuiDebugManager.h>
#endif

#include <algorithm>
#include <cmath>
#include <Features/LineDrawer/LineDrawer.h>

using namespace Engine;

void PenlightCrowd::Initialize(const Vector3& areaCenter,
                               const Vector2& areaSize,
                               int32_t cols, int32_t rows)
{
    maxCount_ = cols * rows;
    lights_.reserve(maxCount_);

    areaCenter_ = areaCenter;
    areaSize_ = { areaSize.x,0.0f,areaSize.y };

    // ペンライト棒メッシュ（細い円柱、+Y方向に伸びる）
    Cylinder rod(0.06f, 0.06f, penlightLength_);
    rod.SetDivide(6);
    rod.SetLoop(true);
    Model* rodModel = rod.Generate("PenlightRod");

    model_ = std::make_unique<InstancedObjectModel>();
    model_->Initialize(rodModel, static_cast<uint32_t>(maxCount_));

    // 格子配置（手前エリアに散らす）
    for (int32_t z = 0; z < rows; ++z)
    {
        for (int32_t x = 0; x < cols; ++x)
        {
            Penlight p;
            float fx = (cols > 1) ? (static_cast<float>(x) / (cols - 1) - 0.5f) : 0.0f;
            float fz = (rows > 1) ? (static_cast<float>(z) / (rows - 1) - 0.5f) : 0.0f;

            // 少しランダムにずらして自然な群衆に
            float jitterX = (std::sin(x * 12.9898f + z * 78.233f) * 0.5f) * (areaSize.x / cols);
            float jitterZ = (std::sin(x * 39.346f + z * 11.135f) * 0.5f) * (areaSize.y / rows);

            p.basePos = areaCenter + Vector3(fx * areaSize.x + jitterX, 0.0f, fz * areaSize.y + jitterZ);
            // 位相を個体ごとにずらして群衆のバラつきを出す
            p.phase = std::sin(x * 1.7f + z * 2.3f) * 3.14159265f;
            // ランダム段階で使う色相を散らす
            p.hue = std::fmod((x * 0.13f + z * 0.07f), 1.0f);

            // 色段階へ進化する個別しきい値（0..1）。バラけさせて群衆がじわじわ染まるように。
            // stage1: 白→ライトブルー / stage2: →ランダム。stage2 の方が必ず後に来るようにする。
            float r1 = std::fmod((x * 0.317f + z * 0.911f), 1.0f);
            float r2 = std::fmod((x * 0.723f + z * 0.251f), 1.0f);
            p.stage1Threshold = 0.15f + 0.45f * r1;            // 0.15〜0.60 で水色化開始
            p.stage2Threshold = p.stage1Threshold + 0.2f + 0.3f * r2; // それより後でランダム化

            lights_.push_back(p);
        }
    }
}

float PenlightCrowd::ComputeEnergy(AudioSpectrum* audioSpectrum)
{
    if (!audioSpectrum)
        return 0.0f;

    if (beginIndex_ == 0 && endIndex_ == 0)
        audioSpectrum->GetSpectrumIndexRange(hzRange_.x, hzRange_.y, beginIndex_, endIndex_);

    audioSpectrum->GetAmplitudesInRange(beginIndex_, endIndex_, spectrumData_);
    if (spectrumData_.empty())
        return 0.0f;

    float peak = *std::max_element(spectrumData_.begin(), spectrumData_.end());
    float db = 20.0f * std::log10(peak + 1e-6f);
    float norm = std::clamp((db + 60.0f) / 70.0f, 0.0f, 1.0f);
    return norm;
}

void PenlightCrowd::Update(float deltaTime, AudioSpectrum* audioSpectrum)
{
    swingTime_ += swingSpeed_ * deltaTime;

    // 低〜中域エネルギーへ滑らかに追従
    float target = ComputeEnergy(audioSpectrum);
    float t = std::clamp(energyResponse_ * deltaTime, 0.0f, 1.0f);
    energyLevel_ += (target - energyLevel_) * t;

    visibleCount_ = std::max(static_cast<int32_t>(maxCount_ * (0.2f + 0.8f * comboRatio_)),
                             std::min(maxCount_, 8));

    float fadeT = std::clamp(fadeSpeed_ * deltaTime, 0.0f, 1.0f);
    for (int32_t i = 0; i < static_cast<int32_t>(lights_.size()); ++i)
    {
        float targetVis = (i < visibleCount_) ? 1.0f : 0.0f;
        lights_[i].visibility += (targetVis - lights_[i].visibility) * fadeT;
    }

#ifdef _DEBUG
    if (ImGuiDebugManager::GetInstance()->Begin("PenlightCrowd Debug"))
    {
        ImGui::Text("ComboRatio: %.2f  Visible: %d / %d", comboRatio_, visibleCount_, maxCount_);
        ImGui::Text("Energy: %.3f", energyLevel_);
        ImGui::DragFloat("Swing Speed", &swingSpeed_, 0.01f, 0.0f, 20.0f);
        ImGui::DragFloat("Swing Base Deg", &swingBaseDeg_, 0.1f, 0.0f, 90.0f);
        ImGui::DragFloat("Swing Energy Deg", &swingEnergyDeg_, 0.1f, 0.0f, 90.0f);
        ImGui::DragFloat("Glow Base", &glowBase_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Glow Energy", &glowEnergy_, 0.01f, 0.0f, 2.0f);

        ImGui::DragFloat3("areasize", &areaSize_.x, 0.01f);
        ImGui::DragFloat3("areaCenter", &areaCenter_.x, 0.01f);
        ImGui::End();
    }
#endif
}

void PenlightCrowd::Draw(const Camera* camera)
{
    if (!model_)
        return;

    model_->Clear();

    float amplitudeDeg = swingBaseDeg_ + swingEnergyDeg_ * energyLevel_;
    float amplitudeRad = amplitudeDeg * 3.14159265f / 180.0f;
    float brightness = glowBase_ + glowEnergy_ * energyLevel_;

    for (const Penlight& p : lights_)
    {
        if (p.visibility <= 0.001f)
            continue;

        float angle = std::sin(swingTime_ + p.phase) * amplitudeRad;
        Vector4 color = ResolveColor(p, comboRatio_, std::clamp(brightness, 0.0f, 1.0f) * p.visibility);
        Vector3 scale = Vector3(1.0f, p.visibility, 1.0f); // visibility=0で潰れ、1で全長

        Matrix4x4 world = MakeAffineMatrix(scale,
                                           Vector3(0.0f, 0.0f, angle),
                                           p.basePos);
        model_->AddInstance(world, color);
    }

    model_->Draw(camera);

    DrawDebugArea();
}

Vector4 PenlightCrowd::ResolveColor(const Penlight& p, float comboRatio, float brightness) const
{
    const Vector4 white     = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 lightBlue = Vector4(0.45f, 0.75f, 1.0f, 1.0f);

    Vector4 baseColor;

    if (comboRatio < p.stage1Threshold)
    {
        baseColor = white;
    }
    else if (comboRatio < p.stage2Threshold)
    {
        float t = (comboRatio - p.stage1Threshold) / (p.stage2Threshold - p.stage1Threshold);
        t = std::clamp(t, 0.0f, 1.0f);
        baseColor = white * (1.0f - t) + lightBlue * t;
    }
    else
    {
        // stage2突入直後はライトブルーから個体色へ滑らかに移行
        HSVA hsv(p.hue * 360.0f, 0.85f, 1.0f, 1.0f);
        RGBA rgb = ColorConverter::ToRGBA(hsv);
        Vector4 randomColor(rgb.r, rgb.g, rgb.b, 1.0f);

        float t = std::clamp((comboRatio - p.stage2Threshold) / 0.15f, 0.0f, 1.0f);
        baseColor = lightBlue * (1.0f - t) + randomColor * t;
    }

    Vector4 color = baseColor * brightness;
    color.w = 1.0f;
    return color;
}

void PenlightCrowd::DrawDebugArea()
{
#ifdef _DEBUG
    Vector3 rt = areaCenter_ + Vector3(areaSize_.x * 0.5f, 0.0f, areaSize_.z * 0.5f);
    Vector3 lt = areaCenter_ + Vector3(-areaSize_.x * 0.5f, 0.0f, areaSize_.z * 0.5f);
    Vector3 rb = areaCenter_ + Vector3(areaSize_.x * 0.5f, 0.0f, -areaSize_.z * 0.5f);
    Vector3 lb = areaCenter_ + Vector3(-areaSize_.x * 0.5f, 0.0f, -areaSize_.z * 0.5f);


    Engine::LineDrawer::GetInstance()->RegisterPoint(rt, rb, Vector4(1, 0, 0, 1));
    Engine::LineDrawer::GetInstance()->RegisterPoint(rb, lb, Vector4(1, 0, 0, 1));
    Engine::LineDrawer::GetInstance()->RegisterPoint(lb, lt, Vector4(1, 0, 0, 1));
    Engine::LineDrawer::GetInstance()->RegisterPoint(lt, rt, Vector4(1, 0, 0, 1));
#endif // _DEBUG
}
