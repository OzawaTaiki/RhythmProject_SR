#include "TitileUI.h"
#include <Features/Event/EventManager.h>
#include <Debug/ImguITools.h>

TitileUI::TitileUI()
{
    eventManager_=EventManager::GetInstance();
    eventManager_->AddEventListener("CloseOptionMenu", this);
    eventManager_->AddEventListener("TitleCameraAnimationEnd", this);
}

TitileUI::~TitileUI()
{
    eventManager_->RemoveEventListener("CloseOptionMenu", this);
    eventManager_->RemoveEventListener("TitleCameraAnimationEnd", this);
}

void TitileUI::Initialize()
{

    titleAnimationSequence_  = std::make_unique<AnimationSequence>("TitleUIAnimation");
    titleAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    InitializeUIElements();
    isActive_ = true;
}

void TitileUI::Update()
{
    if (!isActive_)
        return;
#ifdef _DEBUG
    ImGuiTool::TimeLine("TitleUIAnimation", titleAnimationSequence_.get());
#endif // _DEBUG

    float delta = 0.016f;
    for (auto& [key, element] : animationUIElements_)
    {
        UpdateAnimationUI(key, delta);
    }


    uiGroup_->Update();
}

void TitileUI::Draw()
{
    if (!isActive_)
        return;

    uiGroup_->Draw();
}

void TitileUI::OnEvent(const GameEvent& _event)
{
    if (_event.GetEventType() == "CloseOptionMenu")
    {
        isActive_ = true;
    }
    if (_event.GetEventType() == "TitleCameraAnimationEnd")
    {
    }

}

void TitileUI::InitializeUIElements()
{
    //TODO : デフォコライダーとイベント時こらいだ―を用意してイベント発生時に入れ替える
    auto background = uiGroup_->CreateSprite("title_background");

    auto startParent = uiGroup_->CreateElement<UISprite>("title_startParent");
    auto startButton = uiGroup_->CreateButton("title_start");
    {// スタートボタン

        startButton->SetOnHoverEnter([this]()
                                     {
                                         // TODO : サイズ変更アニメーション
                                         //Vector2 val= titleAnimationSequence_->GetValue<Vector2>("positionOffset");
                                         auto& element = animationUIElements_[TitleUIElement::StartParent];
                                         element.animationLabel = "positionOffset";
                                         element.currentTime = 0.0f;
                                         element.animating = true;
                                     });
        startButton->SetOnHoverExit([this]()
                                    {
                                        // TODO : サイズ変更アニメーション戻し

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
                                          // TODO : サイズ変更アニメーション

                                          auto& element = animationUIElements_[TitleUIElement::OptionsParent];
                                          element.animationLabel = "positionOffset";
                                          element.currentTime = 0.0f;
                                          element.animating = true;
                                      });
        optionButton->SetOnHoverExit([this]()
                                     {
                                         // TODO : サイズ変更アニメーション戻し
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
                                        // TODO : サイズ変更アニメーション
                                        auto& element = animationUIElements_[TitleUIElement::ExitParent];
                                        element.animationLabel = "positionOffset";
                                        element.currentTime = 0.0f;
                                        element.animating = true;
                                    });

        exitButton->SetOnHoverExit([this]()
                                   {
                                       // TODO : サイズ変更アニメーション戻し
                                       auto& element = animationUIElements_[TitleUIElement::ExitParent];
                                       element.animationLabel = "return_posOffset";
                                       element.currentTime = 0.0f;
                                       element.animating = true;
                                   });

        exitButton->SetOnClickEnd([this]()
                                  {
                                      eventManager_->DispatchEvent(GameEvent("RequestExitGame", nullptr));
                                  });
        exitParent->AddChild(exitButton);

        auto exitIcon = uiGroup_->CreateSprite("title_exitIcon");
        exitButton->AddChild(exitIcon);
    }

    background->AddChild(startParent);
    startParent->AddChild(optionParent);
    optionParent->AddChild(exitParent);

    uiElements_[TitleUIElement::Background]     = background;
    uiElements_[TitleUIElement::StartButton]    = startButton;
    uiElements_[TitleUIElement::OptionsButton]  = optionButton;
    uiElements_[TitleUIElement::ExitButton]     = exitButton;

    animationUIElements_[TitleUIElement::StartParent]   = { startParent, startParent->GetPos() };
    animationUIElements_[TitleUIElement::OptionsParent] = { optionParent, optionParent->GetPos() };
    animationUIElements_[TitleUIElement::ExitParent]    = { exitParent, exitParent->GetPos() };
}

void TitileUI::UpdateAnimationUI(TitleUIElement _elem, float _deltaTime)
{
    for (size_t i = ToSizeT(_elem); i < ToSizeT(TitleUIElement::Max); ++i)
    {
        TitleUIElement current = ToTitleUIElement(i);

        if (!animationUIElements_.contains(current))
            continue;

        auto& element = animationUIElements_[current];

        if (!element.animating)
        {
            return;
        }

        element.currentTime += _deltaTime;
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