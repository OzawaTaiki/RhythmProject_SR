#include "TitleUI.h"
#include <Features/Event/EventManager.h>
#include <Debug/ImguITools.h>
#include <Features/UI/Component/UISpriteRenderComponent.h>
#include <Features/UI/Element/UIElement.h>
#include <Features/UI/Element/UIButtonElement.h>
#include <Features/UI/UINavigationManager.h>
TitleUI::TitleUI()
{
    eventManager_=EventManager::GetInstance();
    eventManager_->AddEventListener("CloseOptionMenu", this);
    eventManager_->AddEventListener("TitleCameraAnimationEnd", this);
    eventManager_->AddEventListener("ToSelect", this);
    eventManager_->AddEventListener("ToMenu", this);
}

TitleUI::~TitleUI()
{
    eventManager_->RemoveEventListener("CloseOptionMenu", this);
    eventManager_->RemoveEventListener("TitleCameraAnimationEnd", this);
    eventManager_->RemoveEventListener("ToSelect", this);
    eventManager_->RemoveEventListener("ToMenu", this);
}

void TitleUI::Initialize()
{
    titleAnimationSequence_  = std::make_unique<AnimationSequence>("TitleUIAnimation");
    titleAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

    buttonExpandAnimationSequence_ = std::make_unique<AnimationSequence>("TitleUIExpand");
    buttonExpandAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

    groupChangeAnimationSequence_ = std::make_unique<AnimationSequence>("TitleUIGroupChange");
    groupChangeAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

    focusFrame_ = std::make_unique<FocusFrame>();
    focusFrame_->Initialize();

    InitializeUIElements();
    isActive_ = false;
    isExpanding_= false;
    currentTime_ = 0.0f;
    backgroundElement_->SetEnabled(false);
}

void TitleUI::Update()
{
#ifdef _DEBUG

    ImGuiTool::TimeLine("TitleUI Animation", titleAnimationSequence_.get());
    ImGuiTool::TimeLine("TitleUI Button Expand Animation", buttonExpandAnimationSequence_.get());
    ImGuiTool::TimeLine("TitleUI Group Change Animation", groupChangeAnimationSequence_.get());


#endif // _DEBUG

    float delta = 0.016f;
    if (isExpanding_)
    {
        isActive_ = true;
        currentTime_ += delta;
        if (buttonExpandAnimationSequence_->GetMaxPlayTime() < currentTime_)
        {
            currentTime_ = buttonExpandAnimationSequence_->GetMaxPlayTime();
            isExpanding_ = false;
            UINavigationManager::GetInstance()->SetFocus(uiElementGroups_[currentGroup_].elements[TitleUIElement::StartButton]);
        }

        auto ui = uiElementGroups_[currentGroup_].elements[TitleUIElement::Background];
        Vector2 size = buttonExpandAnimationSequence_->GetValueAtTime<Vector2>("backgroundScale", currentTime_);
        ui->SetSize(size);
        Vector2 pos = buttonExpandAnimationSequence_->GetValueAtTime<Vector2>("buttonPos", currentTime_);
        auto button = uiElementGroups_[currentGroup_].animationElements[TitleUIElement::StartParent];
        button.uiElement->SetPosition(button.basePos + pos);
    }
    backgroundElement_->Update();

    if (!isActive_)
        return;

    if (!isExpanding_)
    {
        for (auto& [key, element] : uiElementGroups_[currentGroup_].animationElements)
        {
            UpdateAnimationUI(key, delta);
        }
    }
    focusFrame_->Update(delta);
}

void TitleUI::Draw()
{
    if (!isActive_)
        return;

    backgroundElement_->Draw();

    focusFrame_->Draw();
}

void TitleUI::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "CloseOptionMenu")
    {
        isActive_ = true;
        UINavigationManager::GetInstance()->SetFocus(uiElements_[TitleUIElement::StartButton]);
        backgroundElement_->SetEnabled(true);
    }
    if (event.GetEventType() == "TitleCameraAnimationEnd")
    {
        isExpanding_ = true;
        backgroundElement_->SetEnabled(true);
    }
    if (event.GetEventType() == "ToSelect")
    {
        currentGroup_ = GroupType::MusicSelect;
    }
    if (event.GetEventType() == "ToMenu")
    {
        currentGroup_ = GroupType::Menu;
    }
}

void TitleUI::EnterButtonExpandAnimation(AnimationUIElement& element)
{
    element.animationLabel = "positionOffset";
    element.currentTime = 0.0f;
    element.animating = true;
    focusFrame_->ChangeTarget(element.uiElement);
}

void TitleUI::ExitButtonExpandAnimation(AnimationUIElement& element)
{
    element.animationLabel = "return_posOffset";
    element.currentTime = 0.0f;
    element.animating = true;
    focusFrame_->ChangeTarget(nullptr);
}

void TitleUI::DisPatchEvent(const std::string& event)
{
    if (event != "RequestStartGame")
        isActive_ = false;
    eventManager_->DispatchEvent(GameEvent(event, nullptr));
}

void TitleUI::InitializeUIElements()
{
    backgroundElement_ = std::make_unique<UIImageElement>("titleUI_background", Vector2(0, 0), Vector2(800, 600));
    backgroundElement_->Initialize();

    {
        // TODO : auto group = uiElementGroups_[GroupType::Menu];
        auto startButton = std::make_unique<UIButtonElement>("title_start", Vector2(100, 100), Vector2(100, 100), "", true);
        startButton->Initialize();
        startButton->SetOnHoverEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::StartParent]); });
        startButton->SetOnHoverExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::StartParent]); });
        startButton->SetOnFocusEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::StartParent]); });
        startButton->SetOnFocusExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::StartParent]); });
        startButton->SetOnClickUp([this]() { DisPatchEvent("ToSelect"); });
        startButton->SetOnClick([this]() { DisPatchEvent("ToSelect"); });

        auto startParent = std::make_unique<UIImageElement>("title_startParent", Vector2(100, 100), Vector2(120, 120));
        startParent->Initialize();

        auto startIcon = std::make_unique<UIImageElement>("title_startIcon", Vector2(0, 0), Vector2(100, 100));
        startIcon->Initialize();
        startButton->AddChild(std::move(startIcon));

        auto optionButton = std::make_unique<UIButtonElement>("title_options", Vector2(100, 250), Vector2(100, 100), "", true);
        optionButton->Initialize();
        optionButton->SetOnHoverEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::OptionsParent]); });
        optionButton->SetOnHoverExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::OptionsParent]); });
        optionButton->SetOnFocusEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::OptionsParent]); });
        optionButton->SetOnFocusExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::OptionsParent]); });
        optionButton->SetOnClickUp([this]() { DisPatchEvent("OpenOptionMenu"); backgroundElement_->SetEnabled(false); });
        optionButton->SetOnClick([this]() { DisPatchEvent("OpenOptionMenu"); backgroundElement_->SetEnabled(false); });

        auto optionParent = std::make_unique<UIImageElement>("title_optionsParent", Vector2(100, 250), Vector2(120, 120));
        optionParent->Initialize();

        auto optionIcon = std::make_unique<UIImageElement>("title_optionsIcon", Vector2(0, 0), Vector2(100, 100));
        optionIcon->Initialize();
        optionButton->AddChild(std::move(optionIcon));

        auto exitButton = std::make_unique<UIButtonElement>("title_exit", Vector2(100, 400), Vector2(100, 100), "", true);
        exitButton->Initialize();
        exitButton->SetOnHoverEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::ExitParent]); });
        exitButton->SetOnHoverExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::ExitParent]); });
        exitButton->SetOnFocusEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::ExitParent]); });
        exitButton->SetOnFocusExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::ExitParent]); });
#ifndef _DEBUG
        exitButton->SetOnClickUp([this]()
                                 {
                                     eventManager_->DispatchEvent(GameEvent("RequestExitGame", nullptr));
                                 });
        exitButton->SetOnClick([this]()
                               {
                                   eventManager_->DispatchEvent(GameEvent("RequestExitGame", nullptr));
                               });
#endif
        auto exitParent = std::make_unique<UIImageElement>("title_exitParent", Vector2(100, 400), Vector2(120, 120));
        exitParent->Initialize();
        auto exitIcon = std::make_unique<UIImageElement>("title_exitIcon", Vector2(0, 0), Vector2(100, 100));
        exitIcon->Initialize();
        exitButton->AddChild(std::move(exitIcon));


        uiElementGroups_[GroupType::Menu].elements[TitleUIElement::Background] = backgroundElement_.get();
        uiElementGroups_[GroupType::Menu].elements[TitleUIElement::StartButton] = startParent->AddChild(std::move(startButton));
        uiElementGroups_[GroupType::Menu].elements[TitleUIElement::OptionsButton] = optionParent->AddChild(std::move(optionButton));
        uiElementGroups_[GroupType::Menu].elements[TitleUIElement::ExitButton] = exitParent->AddChild(std::move(exitButton));

        auto navi1 = uiElementGroups_[GroupType::Menu].elements[TitleUIElement::StartButton]->GetComponent<UINavigationComponent>();
        auto navi2 = uiElementGroups_[GroupType::Menu].elements[TitleUIElement::OptionsButton]->GetComponent<UINavigationComponent>();
        auto navi3 = uiElementGroups_[GroupType::Menu].elements[TitleUIElement::ExitButton]->GetComponent<UINavigationComponent>();

        navi1->SetNavigation(NavigationDirection::Right, uiElementGroups_[GroupType::Menu].elements[TitleUIElement::OptionsButton]);
        navi1->SetFocused(true); // 最初はスタートボタンにフォーカス
        navi2->SetNavigation(NavigationDirection::Left, uiElementGroups_[GroupType::Menu].elements[TitleUIElement::StartButton]);
        navi2->SetNavigation(NavigationDirection::Right, uiElementGroups_[GroupType::Menu].elements[TitleUIElement::ExitButton]);

        navi3->SetNavigation(NavigationDirection::Left, uiElementGroups_[GroupType::Menu].elements[TitleUIElement::OptionsButton]);

        Vector2 startPos = startParent->GetPosition();
        Vector2 optionPos = optionParent->GetPosition();
        Vector2 exitPos = exitParent->GetPosition();


        uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::ExitParent]    = { optionParent->AddChild(std::move(exitParent)), exitPos };
        uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::OptionsParent] = { startParent->AddChild(std::move(optionParent)), optionPos };
        uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::StartParent]  = { backgroundElement_->AddChild(std::move(startParent)) , startPos };


        auto dummyButton = std::make_unique<UIButtonElement>("title_dummyButton", Vector2(0, 0), Vector2(200, 200), "", true);
        dummyButton->Initialize();
        backgroundElement_->AddChild(std::move(dummyButton));

        backgroundElement_->SetEnabled(false);

        focusFrame_->RegisterTargetName(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::StartParent].uiElement);
        focusFrame_->RegisterTargetName(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::OptionsParent].uiElement);
        focusFrame_->RegisterTargetName(uiElementGroups_[GroupType::Menu].animationElements[TitleUIElement::ExitParent].uiElement);
    }


#pragma region MusicSelectUIElements

    {
        auto music1Button = std::make_unique<UIButtonElement>("title_Music1Button", Vector2(100, 100), Vector2(100, 100), "", true);
        music1Button->Initialize();
        music1Button->SetOnHoverEnter([this]()
                                      {
                                          EnterButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music1Parent]);
                                          DisPatchEvent("SelectMusic1");
                                      });
        music1Button->SetOnHoverExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music1Parent]); });
        music1Button->SetOnFocusEnter([this]()
                                      {
                                          EnterButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music1Parent]);
                                          DisPatchEvent("SelectMusic1");
                                      });
        music1Button->SetOnFocusExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music1Parent]); });
        music1Button->SetOnClickUp([this]() { DisPatchEvent("RequestStartGame"); });
        music1Button->SetOnClick([this]() { DisPatchEvent("RequestStartGame"); });

        auto music1Parent = std::make_unique<UIImageElement>("title_Music1Parent", Vector2(100, 100), Vector2(120, 120));
        music1Parent->Initialize();

        auto music1Icon = std::make_unique<UIImageElement>("title_Music1Icon", Vector2(0, 0), Vector2(100, 100));
        music1Icon->Initialize();
        music1Button->AddChild(std::move(music1Icon));

        auto music2Button = std::make_unique<UIButtonElement>("title_Music2Button", Vector2(100, 250), Vector2(100, 100), "", true);
        music2Button->Initialize();
        music2Button->SetOnHoverEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music2Parent]); DisPatchEvent("SelectMusic2"); });
        music2Button->SetOnHoverExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music2Parent]); });
        music2Button->SetOnFocusEnter([this]()
                                      {
                                          EnterButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music2Parent]); DisPatchEvent("SelectMusic2");
                                      });
        music2Button->SetOnFocusExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music2Parent]); });
        music2Button->SetOnClickUp([this]() { DisPatchEvent("SelectMusic2"); });
        music2Button->SetOnClick([this]() { DisPatchEvent("SelectMusic2"); });

        auto music2Parent = std::make_unique<UIImageElement>("title_Music2Parent", Vector2(100, 250), Vector2(120, 120));
        music2Parent->Initialize();
        auto music2Icon = std::make_unique<UIImageElement>("title_Music2Icon", Vector2(0, 0), Vector2(100, 100));
        music2Icon->Initialize();
        music2Button->AddChild(std::move(music2Icon));


        auto returnButton = std::make_unique<UIButtonElement>("title_ReturnButton", Vector2(100, 400), Vector2(100, 100), "", true);
        returnButton->Initialize();
        returnButton->SetOnHoverEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::ReturnParent]); });
        returnButton->SetOnHoverExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::ReturnParent]); });
        returnButton->SetOnFocusEnter([this]() { EnterButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::ReturnParent]); });
        returnButton->SetOnFocusExit([this]() { ExitButtonExpandAnimation(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::ReturnParent]); });
        returnButton->SetOnClickUp([this]() { DisPatchEvent("ToMenu"); });
        returnButton->SetOnClick([this]() { DisPatchEvent("ToMenu"); });
        auto returnParent = std::make_unique<UIImageElement>("title_ReturnParent", Vector2(100, 400), Vector2(120, 120));
        returnParent->Initialize();
        auto returnIcon = std::make_unique<UIImageElement>("title_ReturnIcon", Vector2(0, 0), Vector2(100, 100));
        returnIcon = std::make_unique<UIImageElement>("title_ReturnIcon", Vector2(0, 0), Vector2(100, 100));
        returnIcon->Initialize();
        returnButton->AddChild(std::move(returnIcon));

        uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Music1] = music1Parent->AddChild(std::move(music1Button));
        uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Music2] = music2Parent->AddChild(std::move(music2Button));
        uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Return] = returnParent->AddChild(std::move(returnButton));

        auto navi1 = uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Music1]->GetComponent<UINavigationComponent>();
        auto navi2 = uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Music2]->GetComponent<UINavigationComponent>();
        auto navi3 = uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Return]->GetComponent<UINavigationComponent>();
        navi1->SetNavigation(NavigationDirection::Down, uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Music2]);
        navi2->SetNavigation(NavigationDirection::Up, uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Music1]);
        navi2->SetNavigation(NavigationDirection::Down, uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Return]);
        navi3->SetNavigation(NavigationDirection::Up, uiElementGroups_[GroupType::MusicSelect].elements[TitleUIElement::Music2]);


        Vector2 music1Pos = music1Parent->GetPosition();
        Vector2 music2Pos = music2Parent->GetPosition();
        Vector2 returnPos = returnParent->GetPosition();

        uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::ReturnParent]  = { backgroundElement_->AddChild(std::move(returnParent)) , returnPos };
        uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music2Parent] = { backgroundElement_->AddChild(std::move(music2Parent)) , music2Pos };
        uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music1Parent] = { backgroundElement_->AddChild(std::move(music1Parent)) , music1Pos };

        focusFrame_->RegisterTargetName(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music1Parent].uiElement);
        focusFrame_->RegisterTargetName(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::Music2Parent].uiElement);
        focusFrame_->RegisterTargetName(uiElementGroups_[GroupType::MusicSelect].animationElements[TitleUIElement::ReturnParent].uiElement);

    }

}

void TitleUI::UpdateGroupChange(GroupType newGroup)
{
    if (newGroup == currentGroup_)
        return;

}

void TitleUI::UpdateAnimationUI(TitleUIElement elem, float deltaTime)
{
    for (size_t i = ToSizeT(elem); i < ToSizeT(TitleUIElement::Max); ++i)
    {
        TitleUIElement current = ToTitleUIElement(i);

        if (!uiElementGroups_[currentGroup_].animationElements.contains(current))
            continue;

        auto& element = uiElementGroups_[currentGroup_].animationElements[current];

        if (!element.animating)
        {
            return;
        }

        element.currentTime += deltaTime;
        if (element.currentTime >= titleAnimationSequence_->GetMaxPlayTime())
        {
            element.currentTime = titleAnimationSequence_->GetMaxPlayTime();
            element.animating = false;
        }

        Vector2 val = titleAnimationSequence_->GetValueAtTime<Vector2>(element.animationLabel, element.currentTime);
        element.uiElement->SetPosition(element.basePos + val);
    }
}
//
//void TitileUI::UpdateAnimationUI(AnimationUIElement* _element, float _deltaTime)
//{
//    if (!_element->animating)
//    {
//        return;
//    }
//
//    _element->currentTime += _deltaTime;
//    if (_element->currentTime >= titleAnimationSequence_->GetMaxPlayTime())
//    {
//        _element->currentTime = titleAnimationSequence_->GetMaxPlayTime();
//        _element->animating = false;
//    }
//
//    Vector2 val = titleAnimationSequence_->GetValueAtTime<Vector2>(_element->animationLabel, _element->currentTime);
//    _element->uiElement->SetPos(_element->basePos + val);
//}