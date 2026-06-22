#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/Json/Loader/JsonFileIO.h>
#include <features/Json/JsonBinder.h>
#include "FrequencyBands.h"

namespace Engine
{
class Camera;
class AudioSpectrum;
class LightGroup;
class PointLightComponent;
}


class SpectrumBar
{
public:
    SpectrumBar() = default;
    ~SpectrumBar() = default;

    void Initialize(const std::string& name, size_t id, const json& jsonData);
    void Update(float deltaTime, Engine::AudioSpectrum* audioSpectrum);
    void Draw(const Engine::Camera* camera, ID3D12PipelineState* pso);

    void InitializeSpectrumRange(Engine::AudioSpectrum* audioSpectrum);
private:

    static float CalculateMax(const std::vector<float>& data);
    // 後に個々に計算する

    static Engine::Vector2 GetFrequencyRangeById(size_t id);

    void CreatePointLight(Engine::LightGroup* lightGroup);

    void InitJsonBinder();

private:

    std::unique_ptr<Engine::ObjectModel> model_;
    size_t id_;
    Engine::Vector2 hzRange_;

    std::weak_ptr<Engine::PointLightComponent> pointLight_;
    Engine::Vector3 lightOffset_ = Engine::Vector3(0.0f, 2.0f, 0.0f);

    Engine::Vector4 color_ = { 1.0f,1.0f ,1.0f ,1.0f };

    float maxAmplitude_ = 0.0f;
    std::vector<float> spectrumData_;

    size_t beginIndex_; // 周波数帯域の開始インデックス
    size_t endIndex_; // 周波数帯域の終了インデックス

    std::unique_ptr<Engine::JsonBinder> jsonBinder_;

};
