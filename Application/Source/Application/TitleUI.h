#pragma once

#include <Features/UI/Element/UIImageElement.h>
#include <Features/Event/EventListener.h>
#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/UI/Collider/UIQuadCollider.h>

#include <Application/FocusFrame.h>

#include <memory>

class EventManager;
class TitleUI : public iEventListener
{
private:

    enum class GroupType
    {
        Menu,
        MusicSelect,
    };

    enum class TitleUIElement
    {
        Background,
        StartButton,
        OptionsButton,
        ExitButton,

        StartParent,
        OptionsParent,
        ExitParent,

        Music1,
        Music2,
        Return,
        Music1Parent,
        Music2Parent,
        ReturnParent,

        Max
    };
    struct AnimationUIElement
    {
        UIElement* uiElement = nullptr;

        Vector2 basePos = {};
        float currentTime = 0.0f;
        std::string animationLabel = "";
        bool animating = false;
    };
public:
    TitleUI();
    ~TitleUI();


    void Initialize();

    void Update();

    void Draw();

    void OnEvent(const GameEvent& event) override;

private:

    void EnterButtonExpandAnimation(AnimationUIElement& element);
    void ExitButtonExpandAnimation(AnimationUIElement& element);
    void DisPatchEvent(const std::string& event);

    void InitializeUIElements();

    void UpdateGroupChange(GroupType newGroup);

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

    std::unique_ptr<UIImageElement> backgroundElement_;
    struct elementGroup
    {
        std::map<TitleUIElement, UIElement*> elements;
        std::map<TitleUIElement, AnimationUIElement> animationElements;
    };
    std::map<GroupType, elementGroup> uiElementGroups_;

    GroupType currentGroup_ = GroupType::Menu;
    GroupType nextGroup_ = GroupType::Menu;
    std::unique_ptr<AnimationSequence> groupChangeAnimationSequence_;

    std::unique_ptr<AnimationSequence> titleAnimationSequence_;
    // ボタンが展開されるアニメーション
    std::unique_ptr<AnimationSequence> buttonExpandAnimationSequence_;
    std::unique_ptr<FocusFrame> focusFrame_;
    TitleUIElement animationTarget_ = TitleUIElement::StartButton;


};

