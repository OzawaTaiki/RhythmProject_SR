#include "LaneOutline.h"
#include <Debug/ImGuiDebugManager.h>
#include <Math/MyLib.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <Application/FeedBack/ComboThresholds.h>
#include <System/Time/Time.h>

using namespace Engine;


void LaneOutline::Initialize(Camera* camera)
{
    depthBasedOutLine_ = std::make_unique<DepthBasedOutLine>();
    depthBasedOutLine_->Initialize();

    depthBasedOutLineData_ = DepthBasedOutLineData();
    depthBasedOutLineData_.edgeColor.z = 0.8f;
    depthBasedOutLineData_.edgeWidth = 1.5f;
    depthBasedOutLine_->SetCamera(camera);
    depthBasedOutLine_->SetData(&depthBasedOutLineData_);

    edgeColors_.push_back(Color::White);
    edgeColors_.push_back(ColorConverter::ToHSVA(Color::Yellow));
    edgeColors_.push_back(ColorConverter::ToHSVA(Color::Cyan));

    jsonBinder_ = std::make_unique<JsonBinder>("LaneOutlineConfig", "Resources/Data/Game/");
    jsonBinder_->RegisterVariable("edgeColors", &edgeColors_);
    jsonBinder_->RegisterVariable("intensityRange", &intensityRange_);
}

void LaneOutline::Update(int32_t combo)
{
#ifdef _DEBUG
    if (ImGuiDebugManager::GetInstance()->Begin("LaneOutline"))
    {
        for (size_t i = 0; i < edgeColors_.size(); ++i)
        {
            Vector4 rgba = edgeColors_[i].ToVector4();
            if (ImGui::ColorEdit3(("EdgeColor" + std::to_string(i)).c_str(), &rgba.x))
            {
                edgeColors_[i] = RGBA(rgba);
            }
        }
        ImGui::DragFloatRange2("IntensityRange", &intensityRange_.x, &intensityRange_.y, 0.01f, 0.0f, 2.0f);

        if (ImGui::Button("Save Config"))
        {
            jsonBinder_->Save();
        }
        static bool debug = false;
        static int comb = combo;
        ImGui::Checkbox("Debug Combo", &debug);
        ImGui::DragInt("Combo", &comb);
        ImGui::Text("Combp : %d", combo);
        if (debug)
        {
            combo = comb;
        }

        ImGui::End();
    }

#endif // _DEBUG

    if (!comboThresholds_)
        return;

    size_t maxLevel = comboThresholds_->GetComboLevel(combo);
    int32_t level = comboThresholds_->GetComboLevel(combo);

    // コンボに応じてエッジの色と強度を変更
    size_t index = static_cast<size_t>(level);

    {// 色の設定
        size_t lowerIndex = static_cast<size_t>(std::max(level - 1, 0));
        HSVA upperColor = edgeColors_[index];
        HSVA lowerColor = edgeColors_[lowerIndex];

        float t = std::sin(static_cast<float>((Time::GetCurrentTime)())) * 0.5f + 0.5f;
        Vector4 vec4Color = Lerp(lowerColor.ToVector4(), upperColor.ToVector4(), t);
        depthBasedOutLineData_.edgeColor = vec4Color.xyz();
    }
    {// 強度の設定
        float t = comboThresholds_->GetComboProgress(combo);

        float intensity = Lerp(intensityRange_.x, intensityRange_.y, t / static_cast<float>(maxLevel + 1) * static_cast<float>(index + 1));
        // ↑0からmaxlevelで0~1で補間
        // maxLevelが2の場合
        // level0 -> 0.0 ~ 0.33
        // level1 -> 0.33 ~ 0.66
        // level2 -> 0.66 ~ 1.0

        depthBasedOutLineData_.edgeColorIntensity = intensity;
    }

}

void LaneOutline::Apply(const std::string& input, const std::string& output)
{
    LayerSystem::ApplyPostEffect(input, output, depthBasedOutLine_.get());
}
