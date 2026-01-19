#pragma once
#include <vector>
#include <memory>

#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/Model/ObjectModel.h>


#include <Application/Result/ResultData.h>

// 前方宣言
class Camera;

enum class ModelKind
{
    RankChar, // ランク文字
    SpotLight,// スポットライト
    Speaker, // スピーカー
    //  他エフェクト
    MAX
};

class ResultModelEffect
{
public:
    void Initialize(const ResultData& resultData);
    void Update(float deltaTime);
    void Draw();

    bool IsAnimationEnd() const { return animationSequence_ ? animationSequence_->IsEnd() : true; }

private:

    void CraeteModels(Rank rank);

    void UpdateSpotLight(ObjectModel* model, float angle, float radius,const Vector3& toTarget);
    Vector3 CalcSwingTarget(const Vector3& pos, const Vector3& toTarget, float radius, float angleRad);

    static Vector3 CalculateSpotLightRotation(const Vector3& pos, const Vector3& rotOffset, const Vector3& targetPos);

    static std::string CreateSeqKey(ModelKind kind, size_t index, const std::string& propertyName);

private:

    Camera camera_;

    std::map<ModelKind, std::vector<std::unique_ptr<ObjectModel>>> effectModels_;
    // 遷移後の演出用アニメーションシーケンス
    std::unique_ptr<AnimationSequence> animationSequence_;
    // seqが終わった後のループアニメーション
    std::unique_ptr<AnimationSequence> loopAnimSeq_;
    // 現在再生中のアニメーションシーケンス
    AnimationSequence* currentAnimSeq_ = nullptr;

    float delayTime_ = 2.0f;
    float delayTimer_ = 0.0f;


};