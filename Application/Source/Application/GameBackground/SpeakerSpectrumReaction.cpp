#include "SpeakerSpectrumReaction.h"

#include <Debug/ImGuiDebugManager.h>

void SpeakerSpectrumReaction::Initialize(Engine::ObjectModel* model, const Engine::Vector2& hzRange, int32_t laneIndex)
{
    speaker_ = model;
    hzRange_ = hzRange;
    baseScale_ = speaker_->scale_;

    glowDark_ = Engine::Vector4(0.5f, 0.5f, 0.5f, 1.0f); // 仮の暗いグロー色
    glowBright_ = Engine::Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 仮の明るいグロー色

    jsonBinder_ = std::make_unique<Engine::JsonBinder>("SpeakerSpectrumReaction" + std::to_string(laneIndex), "Resources/Data/Game/GameBackground/");
    jsonBinder_->RegisterVariable("pulseAmount", &pulseAmount_);
    jsonBinder_->RegisterVariable("gain", &gain_);
    jsonBinder_->RegisterVariable("attack", &attack_);
    jsonBinder_->RegisterVariable("release", &release_);
    jsonBinder_->RegisterVariable("glowDark", &glowDark_);
    jsonBinder_->RegisterVariable("glowBright", &glowBright_);
    jsonBinder_->RegisterVariable("hzRange", &hzRange_);
}

void SpeakerSpectrumReaction::Update(float deltaTime, Engine::AudioSpectrum* audioSpectrum, bool noteHitActive)
{
    if (!speaker_ || !audioSpectrum)
        return;

    if (beginIndex_ == 0 && endIndex_ == 0)
    {
        audioSpectrum->GetSpectrumIndexRange(hzRange_.x, hzRange_.y, beginIndex_, endIndex_);
    }

    audioSpectrum->GetAmplitudesInRange(beginIndex_, endIndex_, spectrumData_);
    if (spectrumData_.empty())
        return;
    float peak = *std::max_element(spectrumData_.begin(), spectrumData_.end());
    float db= 20.0f * std::log10f(peak + 1e-6f); // デシベルに変換（小さな値を加えてゼロ除算を防止）
    float target = std::clamp((db + 60.0f) / 70.0f, 0.0f, 1.0f);

    if (target > level_)
    {
        level_ += (target - level_) * attack_;   // 上昇は速く
    }
    else
    {
        level_ += (target - level_) * release_;  // 下降は緩やかに
    }

    boost_ = std::max(0.0f, boost_ - boostDecay_ * deltaTime);
    float effective = std::clamp(level_ * gain_ + boost_, 0.0f, 1.0f);
    speaker_->scale_ = baseScale_ * (1.0f + effective * pulseAmount_);

    if (!noteHitActive)
    {
        for (auto& mat : speaker_->GetMaterials())
        {
            if (mat->GetName() == "diaphragmMaterial")
            {
                mat->SetColor(Lerp(glowDark_, glowBright_, effective));
            }
        }
    }

#ifdef _DEBUG

    if(Engine::ImGuiDebugManager::GetInstance()->Begin("SpeakerSpectrumReaction Debug"))
    {
        ImGui::DragFloat("Pulse Amount", &pulseAmount_, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Gain", &gain_, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Attack", &attack_, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Release", &release_, 0.01f, 0.0f, 5.0f);
        ImGui::ColorEdit4("Glow Dark", &glowDark_.x);
        ImGui::ColorEdit4("Glow Bright", &glowBright_.x);
        ImGui::DragFloat2("Hz Range", &hzRange_.x, 1.0f, 20.0f, 20000.0f);

        if (ImGui::Button("Save"))
        {
            jsonBinder_->Save();
        }

        ImGui::End();
    }

#endif // _DEBUG


}

void SpeakerSpectrumReaction::AddBoost(float amount)
{
    boost_ = std::min(1.0f, boost_ + amount);
}
