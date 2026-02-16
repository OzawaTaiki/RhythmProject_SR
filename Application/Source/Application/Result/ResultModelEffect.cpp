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

    animationSequence_ = std::make_unique<AnimationSequence>("ResultModelEffectSeq");
    animationSequence_->Initialize("Resources/Data/Result/");
    animationSequence_->SetLooping(false);

    loopAnimSeq_ = std::make_unique<AnimationSequence>("ResultModelEffectLoopSeq");
    loopAnimSeq_->Initialize("Resources/Data/Result/");
    loopAnimSeq_->SetLooping(true);

    currentAnimSeq_ = animationSequence_.get();

}

void ResultModelEffect::Update(float deltaTime)
{
#ifdef _DEBUG
    if (ImGuiDebugManager::GetInstance()->Begin("ResultModelEffect"))
    {
        ImGuiTool::TimeLine(currentAnimSeq_->GetLabel().c_str(), currentAnimSeq_);

        ImGui::Text("maxTime: %.2f", animationSequence_->GetMaxPlayTime());
        ImGui::Text("currentTime: %.2f", animationSequence_->GetCurrent());
        ImGui::Text("progress: %.2f %%", (animationSequence_->GetCurrent() / animationSequence_->GetMaxPlayTime()) * 100.0f);

        if (ImGui::Button("Change Seq"))
        {
            if (currentAnimSeq_ == animationSequence_.get())
            {
                currentAnimSeq_ = loopAnimSeq_.get();
            }
            else
            {
                currentAnimSeq_ = animationSequence_.get();
            }
        }

        ImGui::End();
    }
#endif // _DEBUG
    // 遅延時間処理
    delayTimer_+= deltaTime;
    if (delayTimer_ < delayTime_)
    {
        return;
    }

    currentAnimSeq_->Update(deltaTime);

    Vector3 targetPos = effectModels_[ModelKind::RankChar][0]->translate_;

    for (auto& [kind, models] : effectModels_)
    {
        for (size_t i = 0; i < models.size(); ++i)
        {
            auto& model = models[i];

            if (kind == ModelKind::SpotLight)
            {
                Vector3 posOffset = currentAnimSeq_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqPosOffset));
                Vector2 rot = currentAnimSeq_->GetValue<Vector2>(CreateSeqKey(kind, i, kSeqRotOffset));
                Vector3 scale = currentAnimSeq_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqScale));
                float angle = rot.x;
                float radius = rot.y;
                Vector3 toTarget = targetPos - model->translate_;
                toTarget = toTarget.Normalize();

                model->translate_ = posOffset;
                model->scale_ = scale;

                UpdateSpotLight(model.get(), angle, radius, toTarget);
                model->Update();
            }
            else
            {
                // その他モデルの更新
                Vector3 posOffset = currentAnimSeq_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqPosOffset));
                Vector3 rotOffset = currentAnimSeq_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqRotOffset));
                Vector3 scale = currentAnimSeq_->GetValue<Vector3>(CreateSeqKey(kind, i, kSeqScale));

                model->translate_ = posOffset;
                model->euler_= rotOffset;
                model->scale_ = scale;

                model->Update();
            }
        }
    }


    camera_.translate_ = currentAnimSeq_->GetValue<Vector3>("camera_pos");
    camera_.rotate_ = currentAnimSeq_->GetValue<Vector3>("camera_rot");
    camera_.UpdateMatrix();


    if (currentAnimSeq_ == animationSequence_.get() && animationSequence_->IsEnd())
    {
        currentAnimSeq_ = loopAnimSeq_.get();
    }
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
    speaker2->Initialize(kSpeakerModelPath);

    spotLight1->Initialize(kSpotLightModelPath);
    spotLight2->Initialize(kSpotLightModelPath);
    spotLight3->Initialize(kSpotLightModelPath);
    spotLight1->useQuaternion_ = true;
    spotLight2->useQuaternion_ = true;
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

void ResultModelEffect::UpdateSpotLight(ObjectModel* model, float angle, float radius, const Vector3& toTarget)
{
    // toTargetを軸とした円錐運動の注視点を計算し、モデルの回転を更新する

    // 上方向ベクトル
    const Vector3 upVector = Vector3(0, 1, 0);

    // 軸(toTarget)に垂直なベクトルを計算
    Vector3 perpendicular = Cross(toTarget, upVector).Normalize();

    // 半径分のオフセットベクトルを計算
    Vector3 offset = perpendicular * radius;
    Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion(toTarget, angle);

    // オフセットを回転させて注視点を計算
    Vector3 rotatedOffset = q.RotateVector(offset);
    Vector3 lookAtPos = model->translate_ + toTarget + rotatedOffset;

    Vector3 direction = (lookAtPos - model->translate_).Normalize();
    model->quaternion_ = Quaternion::FromToRotation(kSpotLightForward, direction);

}

Vector3 ResultModelEffect::CalcSwingTarget(const Vector3& pos, const Vector3& toTarget, float radius, float angleRad)
{
    // 軸方向を正規化
    Vector3 axis = toTarget.Normalize();

    // 軸に垂直な基底ベクトルを2つ作る
    Vector3 perpendicular1;
    if (fabsf(axis.y) < 0.9f)
        perpendicular1 = Cross(axis, Vector3(0, 1, 0)).Normalize();
    else
        perpendicular1 = Cross(axis, Vector3(1, 0, 0)).Normalize();

    Vector3 perpendicular2 = Cross(axis, perpendicular1).Normalize();

    // 軸周りの円周上の点を計算
    Vector3 offset = (perpendicular1 * cosf(angleRad) +
                      perpendicular2 * sinf(angleRad)) * radius;

    // 注視点を計算（pos + toTarget方向 + offset）
    Vector3 lookAtPos = pos + axis * toTarget.Length() + offset;

    // 注視方向を計算
    Vector3 direction = (lookAtPos - pos).Normalize();

    // オイラー角に変換（モデルがY軸正方向を向いている前提）
    Vector3 result;
    result.x = asinf(-direction.y) - std::numbers::pi_v<float>*0.5f;
    result.y = atan2f(direction.x, direction.z);
    result.z = 0.0f;

    return result;
}

Vector3 ResultModelEffect::CalculateSpotLightRotation(const Vector3& pos, const Vector3& rotOffset, const Vector3& targetPos)
{
    if (rotOffset == Vector3::zero)
        return Vector3::zero;

    Vector3 polar = rotOffset;

    float angle = polar.x;
    float elevation = polar.y;

    float horizontalDist = polar.z * cosf(elevation);
    Vector3 lookAtPos;
    lookAtPos.x = pos.x + horizontalDist * sinf(angle);
    lookAtPos.y = pos.y + polar.z * sinf(elevation);
    lookAtPos.z = pos.z + horizontalDist * cosf(angle);
    lookAtPos+= targetPos;

    Vector3 direction = (lookAtPos - pos).Normalize();

    Vector3 result;
    result.x = asinf(-direction.y);
    result.y = atan2f(direction.x, direction.z);
    result.z = 0.0f;

    return result;
}

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
