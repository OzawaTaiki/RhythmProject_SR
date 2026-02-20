#pragma once
#include <vector>
#include <memory>

#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/Model/ObjectModel.h>


#include <Application/Result/ResultData.h>

// 前方宣言
namespace Engine { class Camera; }

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

    void UpdateSpotLight(Engine::ObjectModel* model, float angle, float radius, const Engine::Vector3& toTarget);
    Engine::Vector3 CalcSwingTarget(const Engine::Vector3& pos, const Engine::Vector3& toTarget, float radius, float angleRad);

    static Engine::Vector3 CalculateSpotLightRotation(const Engine::Vector3& pos, const Engine::Vector3& rotOffset, const Engine::Vector3& targetPos);

    static std::string CreateSeqKey(ModelKind kind, size_t index, const std::string& propertyName);

private:

    Engine::Camera camera_;

    std::map<ModelKind, std::vector<std::unique_ptr<Engine::ObjectModel>>> effectModels_;
    // 遷移後の演出用アニメーションシーケンス
    std::unique_ptr<Engine::AnimationSequence> animationSequence_;
    // seqが終わった後のループアニメーション
    std::unique_ptr<Engine::AnimationSequence> loopAnimSeq_;
    // 現在再生中のアニメーションシーケンス
    Engine::AnimationSequence* currentAnimSeq_ = nullptr;

    float delayTime_ = 2.0f;
    float delayTimer_ = 0.0f;


};
