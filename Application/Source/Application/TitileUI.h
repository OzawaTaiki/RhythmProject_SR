#pragma once

#include <Features/UI/UIGroup.h>
#include <Features/Event/EventListener.h>
#include <Features/Animation/Sequence/AnimationSequence.h>

#include <memory>
#include <Features/UI/Collider/UIQuadCollider.h>

class EventManager;
class TitileUI : public iEventListener
{
private:
public:
    TitileUI();
    ~TitileUI();


    void Initialize();

    void Update();

    void Draw();

    void OnEvent(const GameEvent& _event) override;

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


    void UpdateAnimationUI(TitleUIElement _elem, float _deltaTime);


    size_t ToSizeT(TitleUIElement _element)
    {
        return static_cast<size_t>(_element);
    }
    TitleUIElement ToTitleUIElement(size_t _index)
    {
        return static_cast<TitleUIElement>(_index);
    }

    void SetActive(bool _active) { isActive_= true; }
    bool GetActive() const { return isActive_; }

private:
    bool isActive_ = true;

    EventManager* eventManager_;

    std::unique_ptr<UIGroup> uiGroup_;

    std::map<TitleUIElement, std::shared_ptr<UIBase>> uiElements_;
    std::map<TitleUIElement, AnimationUIElement> animationUIElements_;

    std::unique_ptr<AnimationSequence> titleAnimationSequence_;

    TitleUIElement animationTarget_ = TitleUIElement::StartButton;

};

