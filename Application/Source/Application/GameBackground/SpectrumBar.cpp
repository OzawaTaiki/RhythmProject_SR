#include "SpectrumBar.h"

#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <Math/MyLib.h>
#include <Features/Light/System/LightingSystem.h>
#include <Features/Light/Point/PointLight.h>
#include <Debug/Debug.h>

using namespace Engine;


namespace
{
constexpr float kMaxAmplitude = 1.0f; // 最大振幅の仮定値
constexpr float kBaseIntensity = 5.0f;
constexpr float kMinIntensity = 0.0f;
constexpr float kMaxIntensity = 5.0f;
// ライトの高さ = 基底 * Amplitude
constexpr float kLightHeightScale = 10.0f;

const Vector4 startColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);// 白色
const Vector4 endColor = Vector4(0.0f, 0.8f, 0.8f, 1.0f);// 白よりの水色

}

void SpectrumBar::Initialize(const std::string& name, size_t id, const json& jsonData)
{
    model_ = std::make_unique<ObjectModel>(name);
    std::string filepath = "";

    if (jsonData.contains("file_name") && !jsonData["file_name"].empty())
        filepath = jsonData["file_name"].get<std::string>();

    if (filepath.empty())
        filepath = "cube/cube.obj"; // デフォルトのモデルファイルパス

    model_->Initialize(filepath); // モデルの初期化

    Vector3 scale, rotation, translation;
    if (jsonData.contains("transform"))
    {
        auto& transform = jsonData["transform"];

        if (transform.contains("scale"))
            scale = Vector3(transform["scale"][0], transform["scale"][1], transform["scale"][2]);
        if (transform.contains("rotation"))
            rotation = Vector3(transform["rotation"][0], transform["rotation"][1], transform["rotation"][2]);
        if (transform.contains("transform"))
            translation = Vector3(transform["transform"][0], transform["transform"][1], transform["transform"][2]);
    }
    else
    {
        scale = Vector3(1.0f, 1.0f, 1.0f); // デフォルトのスケール
        rotation = Vector3(0.0f, 0.0f, 0.0f); // デフォルトの回転
        translation = Vector3(0.0f, 0.0f, 0.0f); // デフォルトの位置
    }

    model_->scale_ = scale;
    model_->quaternion_ = Quaternion::EulerToQuaternion(rotation); // 回転をクォータニオンに変換
    model_->translate_ = translation;
    model_->useQuaternion_ = true; // クォータニオンを使用するように設定

    id_ = id;
    hzRange_ = GetFrequencyRangeById(id);

    CreatePointLight(LightingSystem::GetInstance()->GetLightGroup().get());
    InitJsonBinder();
}

void SpectrumBar::Update(float deltaTime, AudioSpectrum* audioSpectrum)
{
    auto pointLightShared = pointLight_.lock();
#ifdef _DEBUG
    ImGui::Begin("Spectrum Bar Info");
    ImGui::PushID(this);
    ImGui::Text("-----ID: %zu ------------------------------", id_);
    ImGui::Text("Frequency Range: %.2f Hz - %.2f Hz", hzRange_.x, hzRange_.y);
    ImGui::Text("Begin Index: %zu", beginIndex_);
    ImGui::Text("End Index: %zu", endIndex_);
    if (pointLightShared)
    {
        ImGui::SeparatorText(pointLightShared->GetName().c_str());
        ImGui::Indent();
        auto& data = pointLightShared->GetData();
        if (ImGui::DragFloat3("Position", &lightOffset_.x, 0.1f))
        {
            pointLightShared->SetOffset(lightOffset_);
        }
        ImGui::ColorEdit3("Color", &data.color.x);
        ImGui::DragFloat("Decay", &data.decay, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat("Intensity", &data.intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Radius", &data.radius, 0.1f, 0.0f, 100.0f);
        ImGui::Checkbox("IsHalf", reinterpret_cast<bool*>(&data.isHalf));
        ImGui::Unindent();
    }
    if (ImGui::DragFloat2("Hz Range", &hzRange_.x, 1.0f, 0.0f, 22000.0f))
        beginIndex_ = endIndex_ = 0;
    if (ImGui::Button("Save")) jsonBinder_->Save();
    ImGui::PopID();
    ImGui::End();


#endif
    if (beginIndex_ == 0 && endIndex_ == 0)
        InitializeSpectrumRange(audioSpectrum);

    audioSpectrum->GetAmplitudesInRange(beginIndex_, endIndex_, spectrumData_);
    float maxAmplitude = CalculateMax(spectrumData_);
    float t = Easing::EaseOutQuart(maxAmplitude);
    float newIntensity = kBaseIntensity * t;
    newIntensity  = std::clamp(newIntensity, kMinIntensity, kMaxAmplitude);
    //pointLightShared->GetData().intensity = newIntensity;
    color_ = Lerp(startColor, endColor * 2.0f, t);
    for (auto& material : model_->GetMaterials())
    {
        if (material->GetName() == "bar_Material")
        {
            material->SetColor(color_);
            auto& uvTrans = material->GetUVTransform();
            uvTrans.SetOffset(1.0f, maxAmplitude);
        }
    }
    pointLightShared->SetOffset(Vector3(0.0f, kLightHeightScale * maxAmplitude, 0.0f));
    pointLightShared->GetData().color = color_;
    pointLightShared->Update();
    model_->Update();
    deltaTime;
}

void SpectrumBar::Draw(const Camera* camera, ID3D12PipelineState* pso)
{
    model_->DrawWithPSO(pso, camera);
}

void SpectrumBar::InitializeSpectrumRange(AudioSpectrum* audioSpectrum)
{
    audioSpectrum->GetSpectrumIndexRange(hzRange_.x, hzRange_.y, beginIndex_, endIndex_);
    audioSpectrum->GetAmplitudesInRange(beginIndex_, endIndex_, spectrumData_);
}

float SpectrumBar::CalculateMax(const std::vector<float>& data)
{
    if (data.empty())
        return 0.0f;

    return *std::max_element(data.begin(), data.end());
}

Vector2 SpectrumBar::GetFrequencyRangeById(size_t id)
{
    switch (id)
    {
        case 0:
            return Vector2(0.0f, FrequencyBands::kSubBass);
        case 1:
            return Vector2(FrequencyBands::kSubBass, FrequencyBands::kBass);
        case 2:
            return Vector2(FrequencyBands::kBass, FrequencyBands::kLowMid);
        case 3:
            return Vector2(FrequencyBands::kLowMid, FrequencyBands::kMid);
        case 4:
            return Vector2(FrequencyBands::kMid, FrequencyBands::kHighMid);
        case 5:
            return Vector2(FrequencyBands::kHighMid, FrequencyBands::kHigh);
        default:
            return Vector2(0.0f, 0.0f);
    }
}

void SpectrumBar::CreatePointLight(LightGroup* lightGroup)
{
    auto pointLight = std::make_shared<PointLightComponent>();

    Debug::Log("Creating Point Light for SpectrumBar ID: " + std::to_string(id_) + "\n");

    lightGroup->AddPointLight("SpectrumBarLight_" + std::to_string(id_), pointLight);
    pointLight->SetParent(&model_->translate_);

    pointLight_ = pointLight;
}

void SpectrumBar::InitJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("SpectrumBar", "Resources/Data/Game/BackGround/");
    std::string id = std::to_string(id_);

    auto pointLightShared = pointLight_.lock();
    if (pointLightShared)
    {
        auto data = pointLightShared->GetData();

        jsonBinder_->RegisterVariable(id + "_PL_Decay", &data.decay);
        jsonBinder_->RegisterVariable(id + "_PL_Intensity", &data.intensity);
        jsonBinder_->RegisterVariable(id + "_PL_Radius", &data.radius);
        jsonBinder_->RegisterVariable(id + "_PL_Color", &data.color);
        jsonBinder_->RegisterVariable(id + "_PL_offset", &lightOffset_);

        pointLightShared->SetOffset(lightOffset_);
    }

    jsonBinder_->RegisterVariable(id + "hzRange", &hzRange_);
}
