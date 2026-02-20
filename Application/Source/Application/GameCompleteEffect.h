#pragma once

#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UI/Element/UITextElement.h>

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

    std::unique_ptr<Engine::AnimationSequence> sequence_;

    std::unique_ptr<Engine::UIImageElement> background_;
    std::unique_ptr<Engine::UIImageElement> banner_;
    std::unique_ptr<Engine::UITextElement> text_;
};
