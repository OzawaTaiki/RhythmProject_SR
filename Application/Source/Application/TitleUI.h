#pragma once

#include <Features/UI/UIGroup.h>
#include <Features/Event/EventListener.h>
#include <Features/Animation/Sequence/AnimationSequence.h>

#include <memory>
#include <Features/UI/Collider/UIQuadCollider.h>

class EventManager;
class TitleUI : public iEventListener
{
private:
public:
    TitleUI();
    ~TitleUI();


    void Initialize();

    void Update();

    void Draw();

    void OnEvent(const GameEvent& event) override;

private:

    void InitializeUIElements();

    enum class TitleUIElement
    {
        Background,
        StartButton,
        OptionsButton,
        ExitButton,

        StartParent,
        OptionsParent,
        ExitParent,

        Max
    };
    struct AnimationUIElement
    {
        std::shared_ptr<UIBase> uiElement = nullptr;

        Vector2 basePos = {};
        float currentTime = 0.0f;
        std::string animationLabel = "";
        bool animating = false;
    };


    void UpdateAnimationUI(TitleUIElement elem, float deltaTime);


    size_t ToSizeT(TitleUIElement element)
    {
        return static_cast<size_t>(element);
    }
    TitleUIElement ToTitleUIElement(size_t index)
    {
        return static_cast<TitleUIElement>(index);
    }

    void SetActive(bool active) { isActive_= active; }
    bool GetActive() const { return isActive_; }

private:
    bool isActive_ = true;
    bool isExpanding_ = false; // ボタンが展開中かどうか
    float currentTime_ = 0.0f;

    EventManager* eventManager_;

    std::unique_ptr<UIGroup> uiGroup_;

    std::map<TitleUIElement, std::shared_ptr<UIBase>> uiElements_;
    std::map<TitleUIElement, AnimationUIElement> animationUIElements_;

    std::unique_ptr<AnimationSequence> titleAnimationSequence_;
    // ボタンが展開されるアニメーション
    std::unique_ptr<AnimationSequence> buttonExpandAnimationSequence_;

    TitleUIElement animationTarget_ = TitleUIElement::StartButton;

};

