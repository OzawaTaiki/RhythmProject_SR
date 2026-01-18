#include "ResultModelEffect.h"

#include <Debug/ImguITools.h>
#include <Debug/ImGuiDebugManager.h>

namespace
{
const std::string kSpeakerModelPath = "Speaker/Speaker.gltf";
const std::string kSpotLightModelPath = "Spotlight/Spotlight.obj";
const std::string kRankCharModelBasePath = "Char/Char_";
const std::string kRankCharModelPathExtension = ".obj";

// seq_Key用固定文字列
const std::string kSeqSpeakerKey = "Sp_";
const std::string kSeqSpotLightKey = "SL_";
const std::string kSeqRankCharKey = "RC_";

const std::string kSeqPosOffset = "PosOffset_";
const std::string kSeqRotOffset = "RotOffset_";
const std::string kSeqScale = "Scale_";

// スポットライトの正面ベクトル
const Vector3 kSpotLightForward = Vector3(0, 0, 1);
}


void ResultModelEffect::Initialize(const ResultData& resultData)
{
    camera_.Initialize();

    CraeteModels(resultData.rank);

    animationSequence_ = std::make_unique<AnimationSequence>("ResultModelEffectSeq_" + RankCalculator::GetRankString(resultData.rank));
    animationSequence_->Initialize("Resources/Data/Result/");
}

void ResultModelEffect::Update(float deltaTime)
{
#ifdef _DEBUG
    if(ImGuiDebugManager::GetInstance()->Begin("ResultModelEffect"))
    {
        ImGuiTool::TimeLine(animationSequence_->GetLabel().c_str(), animationSequence_.get());

        ImGui::Text("maxTime: %.2f", animationSequence_->GetMaxPlayTime());
        ImGui::Text("currentTime: %.2f", animationSequence_->GetCurrent());
        ImGui::Text("progress: %.2f %%", (animationSequence_->GetCurrent() / animationSequence_->GetMaxPlayTime()) * 100.0f);

        ImGui::End();
    }
#endif // _DEBUG
    // 遅延時間処理
    delayTimer_+= deltaTime;
    if (delayTimer_ < delayTime_)
    {
        return;
    }

    animationSequence_->Update(deltaTime);

    for (auto& [kind, models] : effectModels_)
    {
        for (size_t i = 0; i < models.size(); ++i)
        {
            auto& model = models[i];
            // 位置オフセット取得
            Vector3 posOffset = animationSequence_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqPosOffset));
            Vector3 rotOffset = animationSequence_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqRotOffset));
            Vector3 scale = animationSequence_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqScale));

            model->translate_ = posOffset;
            model->quaternion_ = Quaternion::FromToRotation(kSpotLightForward, rotOffset.Normalize());
            model->scale_ = scale;

            model->Update();
        }
    }

    camera_.translate_ = animationSequence_->GetValue<Vector3>("camera_pos");
    camera_.rotate_ = animationSequence_->GetValue<Vector3>("camera_rot");
    camera_.UpdateMatrix();
}

void ResultModelEffect::Draw()
{
    for (const auto& [kind, models] : effectModels_)
    {
        for (const auto& model : models)
        {
            model->Draw(&camera_);
        }
    }
}

void ResultModelEffect::CraeteModels(Rank rank)
{
    auto speaker1 = std::make_unique<ObjectModel>("result_speaker1");
    auto speaker2 = std::make_unique<ObjectModel>("result_speaker2");
    auto spotLight1 = std::make_unique<ObjectModel>("result_spotlight");
    auto spotLight2 = std::make_unique<ObjectModel>("result_spotlight");
    auto spotLight3 = std::make_unique<ObjectModel>("result_spotlight");
    auto charModel = std::make_unique<ObjectModel>("result_rankChar");

    speaker1->Initialize(kSpeakerModelPath);
    speaker1->useQuaternion_ = true;
    speaker2->Initialize(kSpeakerModelPath);
    speaker2->useQuaternion_ = true;
    spotLight1->Initialize(kSpotLightModelPath);
    spotLight1->useQuaternion_ = true;
    spotLight2->Initialize(kSpotLightModelPath);
    spotLight2->useQuaternion_ = true;
    spotLight3->Initialize(kSpotLightModelPath);
    spotLight3->useQuaternion_ = true;

    const std::string rankCharPath = kRankCharModelBasePath + RankCalculator::GetRankString(rank) + kRankCharModelPathExtension;
    charModel->Initialize(rankCharPath);

    effectModels_[ModelKind::Speaker].push_back(std::move(speaker1));
    effectModels_[ModelKind::Speaker].push_back(std::move(speaker2));
    effectModels_[ModelKind::SpotLight].push_back(std::move(spotLight1));
    effectModels_[ModelKind::SpotLight].push_back(std::move(spotLight2));
    effectModels_[ModelKind::SpotLight].push_back(std::move(spotLight3));
    effectModels_[ModelKind::RankChar].push_back(std::move(charModel));
}

/*
// ルール
// modelKey + transformKey + index
*/

std::string ResultModelEffect::CreateSeqKey(ModelKind kind, size_t index, const std::string& propertyName)
{
    std::string key;
    switch (kind)
    {
        case ModelKind::RankChar:
            key += kSeqRankCharKey;
            break;
        case ModelKind::Speaker:
            key += kSeqSpeakerKey;
            break;
        case ModelKind::SpotLight:
            key += kSeqSpotLightKey;
            break;
        default:
            break;
    }
    key += std::to_string(index) + "_";
    key += propertyName;

    return key;
}
