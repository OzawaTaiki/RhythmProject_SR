#include "SpotLightBeam.h"

#include <Features/Model/ObjectModel.h>
#include <Features/Model/Primitive/Cylinder.h>
#include <Features/Light/Spot/SpotLight.h>
#include <Features/Light/System/LightingSystem.h>
#include <Features/Light/Group/LightGroup.h>
#include <Math/Quaternion/Quaternion.h>
#include <Math/Vector/VectorFunction.h>
#include <Math/Color/Color.h>
#include <Math/MyLib.h>

#include <algorithm>
#include <cmath>

using namespace Engine;

void SpotLightBeam::Initialize(const std::string& name,
                               const Vector3& position,
                               const Vector3& target,
                               const Vector4& color)
{
    position_ = position;
    color_ = color;

    Vector3 toTarget = target - position;
    float dist = Length(toTarget);
    if (dist < 0.0001f)
        baseDirection_ = Vector3(0.0f, -1.0f, 0.0f);
    else
        baseDirection_ = toTarget * (1.0f / dist);
    direction_ = baseDirection_;

    // --- 照明（SpotLightComponent）---
    light_ = std::make_shared<SpotLightComponent>();
    light_->SetPosition(position_);
    light_->SetDirection(direction_);
    light_->SetColor(color_);
    light_->SetIntensity(baseIntensity_);
    light_->SetDistance(length_ * 1.5f);
    light_->SetCastShadow(false);

    auto lightGroup = LightingSystem::GetInstance()->GetLightGroup();
    if (lightGroup)
        lightGroup->AddSpotLight("Beam_" + name, light_);

    // --- 可視光線（円錐メッシュ）---
    // Cylinder: 局所+Y軸方向。y=0(根元=スピーカー)〜y=height(先端=上空)
    // 根元を細く・先(上空)を太くして、上に行くほど広がる光柱にする
    Cylinder cone(1.4f, 0.15f, length_); // topRadius(先端=上空,太), bottomRadius(根元=スピーカー,細), height
    cone.SetDivide(20);
    cone.SetLoop(true);
    Model* coneModel = cone.Generate("LightBeamMesh_" + name);

    beam_ = std::make_unique<ObjectModel>("LightBeam_" + name);
    beam_->Initialize(coneModel);

    // 局所+Yを direction_ に向ける
    // ※Cylinderは根元(y=0)が太い→根元を光源に置きたいので、+Yを「光の進行方向」に合わせる
    beam_->translate_ = position_;
    beam_->quaternion_ = Quaternion::FromToRotation(Vector3(0.0f, 1.0f, 0.0f), direction_);
    beam_->useQuaternion_ = true;
    beam_->scale_ = Vector3::one;
    baseBeamScale_ = beam_->scale_;
}

void SpotLightBeam::Update(float deltaTime)
{
    // 駆動値へ滑らかに追従
    float t = std::clamp(energyResponse_ * deltaTime, 0.0f, 1.0f);
    energyLevel_ += (energyTarget_ - energyLevel_) * t;

    // バースト減衰
    burstLevel_ = std::max(0.0f, burstLevel_ - burstDecay_ * deltaTime);

    // 明るさ＝常時下限(baseline) ＋ 駆動値 ＋ バースト
    float combined = std::clamp(baseline_ + energyLevel_ + burstLevel_, 0.0f, 1.0f);

    // 動的カラー：色相=時間サイクル＋FFT帯域、彩度明度=energy
    if (useDynamicColor_)
    {
        hue_ += hueCycleSpeed_ * deltaTime;
        // 時間サイクル ＋ 支配帯域(0..1 を 0..300度へ) を合成
        float h = std::fmod(hue_ + dominantBand_ * 300.0f, 360.0f);
        if (h < 0.0f) h += 360.0f;
        float sat = comboRatio_; // コンボ比率のみで彩度を決定（音エネルギーで揺れないように）
        sat = std::clamp(sat, 0.0f, 1.0f);
        float val = std::clamp(0.6f + 0.4f * combined, 0.0f, 1.0f);   // energyで明るく
        RGBA rgb = ColorConverter::ToRGBA(HSVA(h, sat, val, 1.0f));
        color_ = Vector4(rgb.r, rgb.g, rgb.b, 1.0f);
    }

    // 太さ脈動：energyでビームの太さ(X/Z)が増える
    if (beam_ && thicknessPulse_ > 0.0001f)
    {
        float scaleXZ = 1.0f + combined * thicknessPulse_;
        beam_->scale_ = Vector3(baseBeamScale_.x * scaleXZ,
                                baseBeamScale_.y,
                                baseBeamScale_.z * scaleXZ);
    }

    // 左右スイング：BPM同期の基本波 + 半周期の速い複合波で有機的な揺れに
    if (swingAmplitudeDeg_ > 0.0001f)
    {
        swingPhase_ += swingSpeed_ * deltaTime;
        // 2本目は基本波の1.5倍速・振幅1/3・位相オフセット付きで複合
        swingPhase2_ += swingSpeed_ * 1.5f * deltaTime;

        float ampRad = swingAmplitudeDeg_ * 3.14159265f / 180.0f;
        float wave1 = std::sin(swingPhase_ + swingPhaseOffset_);
        float wave2 = std::sin(swingPhase2_ + swingPhaseOffset_ * 1.7f) * 0.33f;

        // energyで振れ幅も変化（盛り上がると激しく振る）
        float ampScale = 1.0f + energyLevel_ * 0.8f;

        float angle = (wave1 + wave2) * ampRad * ampScale;

        Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), angle);
        direction_ = q.RotateVector(baseDirection_);
    }
    else
    {
        direction_ = baseDirection_;
    }

    // ライト輝度・向きへ反映
    if (light_)
    {
        float intensity = baseIntensity_ + combined * 2.0f; // 照明の効きも控えめに
        light_->SetIntensity(intensity);
        light_->SetColor(color_);
        light_->SetDirection(direction_);
        light_->Update();
    }

    // 光線メッシュの向きをスイングに追従
    if (beam_)
    {
        beam_->quaternion_ = Quaternion::FromToRotation(Vector3(0.0f, 1.0f, 0.0f), direction_);
        beam_->Update();
    }
}

void SpotLightBeam::Draw(const Camera* camera, ID3D12PipelineState* beamPso)
{
    if (!beam_)
        return;

    // baseline＋駆動値＋バーストの合算で明るさが決まる
    float brightness = std::clamp(baseline_ + energyLevel_ + burstLevel_, 0.0f, 1.0f);
    // 可視光線の主張を抑える減衰（明るさだけ控えめに）
    brightness *= beamBrightnessScale_;
    if (brightness <= 0.001f)
        return;

    Vector4 beamColor = color_ * brightness;
    beamColor.w = brightness;
    beam_->DrawWithPSO(beamPso, camera, beamColor);
}

void SpotLightBeam::Burst(float amount)
{
    burstLevel_ = std::min(1.0f, burstLevel_ + amount);
}

void SpotLightBeam::SetBPM(float bpm)
{
    const float kBeatsPerCycle = 8.0f; // 8拍で1往復
    swingSpeed_ = 3.14159265f * bpm / (60.0f * kBeatsPerCycle);
}
