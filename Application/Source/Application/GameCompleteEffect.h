#pragma once

#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/UI/UIImageElement.h>
#include <Features/UI/UITextElement.h>

#include <Application/Note/Judge/JudgeType.h>

class GameCompleteEffect
{
public:
    GameCompleteEffect() = default;
    ~GameCompleteEffect() = default;

    void Initialize();

    void StartEffect(const std::map<JudgeType, int32_t>& judgeResult);
    void Update(float deltaTime);
    void Draw();
    bool IsEffectComplete() const;

private:

    void UpdateElements();

    std::string GenerateResultText(const std::map<JudgeType, int32_t>& judgeResult) const;

private:
    bool isEffectActive_ = false;
    bool isComplete_ = false;

    std::unique_ptr<AnimationSequence> sequence_;

    std::unique_ptr<UIImageElement> background_;
    std::unique_ptr<UIImageElement> banner_;
    std::unique_ptr<UITextElement> text_;
};