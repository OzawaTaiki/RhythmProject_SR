#include "TitleUI.h"
#include <Features/Event/EventManager.h>
#include <Debug/ImguITools.h>
#include <Features/UI/UITextBox.h>

TitleUI::TitleUI()
{
    eventManager_=EventManager::GetInstance();
    eventManager_->AddEventListener("CloseOptionMenu", this);
    eventManager_->AddEventListener("TitleCameraAnimationEnd", this);
}

TitleUI::~TitleUI()
{
    eventManager_->RemoveEventListener("CloseOptionMenu", this);
    eventManager_->RemoveEventListener("TitleCameraAnimationEnd", this);
}

void TitleUI::Initialize()
{

    titleAnimationSequence_  = std::make_unique<AnimationSequence>("TitleUIAnimation");
    titleAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

    buttonExpandAnimationSequence_ = std::make_unique<AnimationSequence>("TitleUIExpand");
    buttonExpandAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    InitializeUIElements();
    isActive_ = false;
    isExpanding_= false;
    currentTime_ = 0.0f;
}

void TitleUI::Update()
{

    float delta = 0.016f;
    if (isExpanding_)
    {
        isActive_ = true;
        currentTime_ += delta;
        if (buttonExpandAnimationSequence_->GetMaxPlayTime() < currentTime_)
        {
            currentTime_ = buttonExpandAnimationSequence_->GetMaxPlayTime();
            isExpanding_ = false;
        }

        auto ui = uiElements_[TitleUIElement::Background];
        Vector2 size = buttonExpandAnimationSequence_->GetValueAtTime<Vector2>("backgroundScale", currentTime_);
        ui->SetSize(size);
        Vector2 pos = buttonExpandAnimationSequence_->GetValueAtTime<Vector2>("buttonPos", currentTime_);
        auto button = animationUIElements_[TitleUIElement::StartParent];
        button.uiElement->SetPos(button.basePos + pos);
    }

    if (!isActive_)
        return;
//
//#ifdef _DEBUG
//    ImGuiTool::TimeLine("TitleUIAnimation", titleAnimationSequence_.get());
//    ImGuiTool::TimeLine("TitleUIExpand", buttonExpandAnimationSequence_.get());
//    ImGui::Begin("TitleUI::", nullptr, ImGuiWindowFlags_NoTitleBar);
//    if (ImGui::Button("Init"))
//        Initialize();
//    ImGui::End();
//
//#endif // _DEBUG

    if(!isExpanding_)
    {
        for (auto& [key, element] : animationUIElements_)
        {
            UpdateAnimationUI(key, delta);
        }

        uiGroup_->Update();
    }
}

void TitleUI::Draw()
{
    if (!isActive_)
        return;

    uiGroup_->Draw();
}

void TitleUI::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "CloseOptionMenu")
    {
        isActive_ = true;
    }
    if (event.GetEventType() == "TitleCameraAnimationEnd")
    {
        isExpanding_ = true;
    }

}

void TitleUI::InitializeUIElements()
{
    auto background = uiGroup_->CreateSprite("title_background");

    auto startParent = uiGroup_->CreateElement<UISprite>("title_startParent");
    auto startButton = uiGroup_->CreateButton("title_start");
    {// スタートボタン

        startButton->SetOnHoverEnter([this]()
                                     {
                                         auto& element = animationUIElements_[TitleUIElement::StartParent];
                                         element.animationLabel = "positionOffset";
                                         element.currentTime = 0.0f;
                                         element.animating = true;
                                     });
        startButton->SetOnHoverExit([this]()
                                    {
                                        auto& element = animationUIElements_[TitleUIElement::StartParent];
                                        element.animationLabel = "return_posOffset";
                                        element.currentTime = 0.0f;
                                        element.animating = true;
                                    });
        startButton->SetOnClickEnd([this]()
                                   {
                                       eventManager_->DispatchEvent(GameEvent("RequestStartGame", nullptr));
                                   });

        startParent->AddChild(startButton);

        auto startIcon = uiGroup_->CreateSprite("title_startIcon");

        startButton->AddChild(startIcon);
    }

    auto optionParent = uiGroup_->CreateElement<UISprite>("title_optionsParent");
    auto optionButton = uiGroup_->CreateButton("title_options");
    {// オプションボタン
        optionButton->SetOnHoverEnter([this]()
                                      {
                                          auto& element = animationUIElements_[TitleUIElement::OptionsParent];
                                          element.animationLabel = "positionOffset";
                                          element.currentTime = 0.0f;
                                          element.animating = true;
                                      });
        optionButton->SetOnHoverExit([this]()
                                     {
                                         auto& element = animationUIElements_[TitleUIElement::OptionsParent];
                                         element.animationLabel = "return_posOffset";
                                         element.currentTime = 0.0f;
                                         element.animating = true;
                                     });
        optionButton->SetOnClickEnd([this]()
                                    {
                                        eventManager_->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));
                                        isActive_ = false;
                                    });
        optionParent->AddChild(optionButton);

        auto optionIcon = uiGroup_->CreateSprite("title_optionsIcon");
        optionButton->AddChild(optionIcon);

    }

    auto exitParent = uiGroup_->CreateElement<UISprite>("title_exitParent");
    auto exitButton = uiGroup_->CreateButton("title_exit");
    {
        exitButton->SetOnHoverEnter([this]()
                                    {
                                        auto& element = animationUIElements_[TitleUIElement::ExitParent];
                                        element.animationLabel = "positionOffset";
                                        element.currentTime = 0.0f;
                                        element.animating = true;
                                    });

        exitButton->SetOnHoverExit([this]()
                                   {
                                       auto& element = animationUIElements_[TitleUIElement::ExitParent];
                                       element.animationLabel = "return_posOffset";
                                       element.currentTime = 0.0f;
                                       element.animating = true;
                                   });

        exitButton->SetOnClickEnd([this]()
                                  {
#ifndef _DEBUG // デバッグビルド時は終了しない
                                      eventManager_->DispatchEvent(GameEvent("RequestExitGame", nullptr));
#endif // _DEBUG
                                  });
        exitParent->AddChild(exitButton);

        auto exitIcon = uiGroup_->CreateSprite("title_exitIcon");
        exitButton->AddChild(exitIcon);
    }

    background->AddChild(startParent);
    startParent->AddChild(optionParent);
    optionParent->AddChild(exitParent);

    // スタートボタンとリングの重なっている部分を無効化するためのダミーボタン
    // なのでコールバックは設定しない
    auto dummyButton = uiGroup_->CreateButton("title_dummyButton");
    background->AddChild(dummyButton);

    uiElements_[TitleUIElement::Background]     = background;
    uiElements_[TitleUIElement::StartButton]    = startButton;
    uiElements_[TitleUIElement::OptionsButton]  = optionButton;
    uiElements_[TitleUIElement::ExitButton]     = exitButton;

    animationUIElements_[TitleUIElement::StartParent]   = { startParent, startParent->GetPos() };
    animationUIElements_[TitleUIElement::OptionsParent] = { optionParent, optionParent->GetPos() };
    animationUIElements_[TitleUIElement::ExitParent]    = { exitParent, exitParent->GetPos() };
}

void TitleUI::UpdateAnimationUI(TitleUIElement elem, float deltaTime)
{
    for (size_t i = ToSizeT(elem); i < ToSizeT(TitleUIElement::Max); ++i)
    {
        TitleUIElement current = ToTitleUIElement(i);

        if (!animationUIElements_.contains(current))
            continue;

        auto& element = animationUIElements_[current];

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
        element.uiElement->SetPos(element.basePos + val);
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