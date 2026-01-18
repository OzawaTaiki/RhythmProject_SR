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

    static std::string CreateSeqKey(ModelKind kind, size_t index, const std::string& propertyName);

private:

    Camera camera_;

    std::map<ModelKind, std::vector<std::unique_ptr<ObjectModel>>> effectModels_;
    std::unique_ptr<AnimationSequence> animationSequence_;

    float delayTime_ = 2.0f;
    float delayTimer_ = 0.0f;


};