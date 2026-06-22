#pragma once

#include <Features/Model/ObjectModel.h>

#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <Features/Json/JsonBinder.h>

class SpeakerSpectrumReaction
{
public:

    void Initialize(Engine::ObjectModel* model, const Engine::Vector2& hzRange, int32_t laneIndex);

    void Update(float deltaTime, Engine::AudioSpectrum* audioSpectrum, bool noteHitActive);

    void AddBoost(float amount);
private:

    Engine::ObjectModel* speaker_ = nullptr;
    Engine::Vector2 hzRange_ = {};
    size_t beginIndex_ = 0;
    size_t endIndex_ = 0;
    float level_ = 0.0f;
    float boost_ = 0.0f;
    Engine::Vector3 baseScale_ = {};
    std::vector<float> spectrumData_;

    float pulseAmount_ = 0.0f;
    float gain_ = 1.0f;
    float attack_ = 0.5f;
    float release_ = 0.1f;
    float boostDecay_ = 0.5f;
    Engine::Vector4 glowDark_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    Engine::Vector4 glowBright_ = { 1.0f, 1.0f, 1.0f, 1.0f };

    std::unique_ptr<Engine::JsonBinder> jsonBinder_;
};