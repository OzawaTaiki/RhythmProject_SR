#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/Json/Loader/JsonFileIO.h>
#include <features/Json/JsonBinder.h>


namespace Engine { class Camera; class AudioSpectrum; class LightGroup; }
class PointLightComponent;

struct FrequencyBands
{
    static constexpr float kSubBass    = 60.0f;    // 0    - 60   Hz (重低音)
    static constexpr float kBass       = 250.0f;   // 60   - 250  Hz (ベース)
    static constexpr float kLowMid     = 500.0f;   // 250  - 500  Hz (低域ミッド)
    static constexpr float kMid        = 2000.0f;  // 500  - 2000 Hz (中域)
    static constexpr float kHighMid    = 6000.0f;  // 2000 - 6000 Hz (高域ミッド)
    static constexpr float kHigh       = 22000.0f; // 6000 - 22000Hz (高音)
};


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

    std::weak_ptr<PointLightComponent> pointLight_;
    Engine::Vector3 lightOffset_ = Engine::Vector3(0.0f, 2.0f, 0.0f);

    Engine::Vector4 color_ = { 1.0f,1.0f ,1.0f ,1.0f };

    float maxAmplitude_ = 0.0f;
    std::vector<float> spectrumData_;

    size_t beginIndex_; // 周波数帯域の開始インデックス
    size_t endIndex_; // 周波数帯域の終了インデックス

    std::unique_ptr<Engine::JsonBinder> jsonBinder_;

};
