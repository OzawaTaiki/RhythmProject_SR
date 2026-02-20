#include "TitleUI.h"
#include <Debug/ImguITools.h>
#include <Features/Event/EventManager.h>
#include <Features/UI/Component/UISpriteRenderComponent.h>
#include <Features/UI/Element/UIButtonElement.h>
#include <Features/UI/Element/UIElement.h>
#include <Features/UI/UINavigationManager.h>

using namespace Engine;


TitleUI::TitleUI() {
  eventManager_ = EventManager::GetInstance();
  eventManager_->AddEventListener("CloseOptionMenu", this);
  eventManager_->AddEventListener("TitleCameraAnimationEnd", this);
}

TitleUI::~TitleUI() {
  eventManager_->RemoveEventListener("CloseOptionMenu", this);
  eventManager_->RemoveEventListener("TitleCameraAnimationEnd", this);
}

void TitleUI::Initialize() {
  titleAnimationSequence_ =
      std::make_unique<AnimationSequence>("TitleUIAnimation");
  titleAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

  buttonExpandAnimationSequence_ =
      std::make_unique<AnimationSequence>("TitleUIExpand");
  buttonExpandAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

  focusFrame_ = std::make_unique<FocusFrame>();
  focusFrame_->Initialize();

  InitializeUIElements();
  isActive_ = false;
  isExpanding_ = false;
  currentTime_ = 0.0f;
  backgroundElement_->SetEnabled(false);
}

void TitleUI::Update() {
#ifdef _DEBUG

  ImGuiTool::TimeLine("TitleUI Animation", titleAnimationSequence_.get());

#endif // _DEBUG

  float delta = 0.016f;
  if (isExpanding_) {
    isActive_ = true;
    currentTime_ += delta;
    if (buttonExpandAnimationSequence_->GetMaxPlayTime() < currentTime_) {
      currentTime_ = buttonExpandAnimationSequence_->GetMaxPlayTime();
      isExpanding_ = false;
      UINavigationManager::GetInstance()->SetFocus(
          uiElements_[TitleUIElement::StartButton]);
    }

    auto ui = uiElements_[TitleUIElement::Background];
    Vector2 size = buttonExpandAnimationSequence_->GetValueAtTime<Vector2>(
        "backgroundScale", currentTime_);
    ui->SetSize(size);
    Vector2 pos = buttonExpandAnimationSequence_->GetValueAtTime<Vector2>(
        "buttonPos", currentTime_);
    auto button = animationUIElements_[TitleUIElement::StartParent];
    button.uiElement->SetPosition(button.basePos + pos);
  }
  backgroundElement_->Update();

  if (!isActive_)
    return;

  if (!isExpanding_) {
    for (auto &[key, element] : animationUIElements_) {
      UpdateAnimationUI(key, delta);
    }
  }
  focusFrame_->Update(delta);
}

void TitleUI::Draw() {
  if (!isActive_)
    return;

  backgroundElement_->Draw();

  focusFrame_->Draw();
}

void TitleUI::OnEvent(const GameEvent &event) {
  if (event.GetEventType() == "CloseOptionMenu") {
    isActive_ = true;
    UINavigationManager::GetInstance()->SetFocus(
        uiElements_[TitleUIElement::StartButton]);
    backgroundElement_->SetEnabled(true);
  }
  if (event.GetEventType() == "TitleCameraAnimationEnd") {
    isExpanding_ = true;
    backgroundElement_->SetEnabled(true);
  }
}

void TitleUI::EnterButtonExpandAnimation(AnimationUIElement &element) {
  element.animationLabel = "positionOffset";
  element.currentTime = 0.0f;
  element.animating = true;
  focusFrame_->ChangeTarget(element.uiElement);
}

void TitleUI::ExitButtonExpandAnimation(AnimationUIElement &element) {
  element.animationLabel = "return_posOffset";
  element.currentTime = 0.0f;
  element.animating = true;
  focusFrame_->ChangeTarget(nullptr);
}

void TitleUI::DisPatchEvent(const std::string &event) {
  if (event != "RequestStartGame")
    isActive_ = false;
  eventManager_->DispatchEvent(GameEvent(event, nullptr));
}

void TitleUI::InitializeUIElements() {
  backgroundElement_ = std::make_unique<UIImageElement>(
      "titleUI_background", Vector2(0, 0), Vector2(800, 600));
  backgroundElement_->Initialize();

  auto startButton = std::make_unique<UIButtonElement>(
      "title_start", Vector2(100, 100), Vector2(100, 100), "", true);
  startButton->Initialize();
  startButton->SetOnHoverEnter([this]() {
    EnterButtonExpandAnimation(
        animationUIElements_[TitleUIElement::StartParent]);
  });
  startButton->SetOnHoverExit([this]() {
    ExitButtonExpandAnimation(
        animationUIElements_[TitleUIElement::StartParent]);
  });
  startButton->SetOnFocusEnter([this]() {
    EnterButtonExpandAnimation(
        animationUIElements_[TitleUIElement::StartParent]);
  });
  startButton->SetOnFocusExit([this]() {
    ExitButtonExpandAnimation(
        animationUIElements_[TitleUIElement::StartParent]);
  });
  startButton->SetOnClickUp([this]() { DisPatchEvent("RequestStartGame"); });
  startButton->SetOnClick([this]() { DisPatchEvent("RequestStartGame"); });

  auto startParent = std::make_unique<UIImageElement>(
      "title_startParent", Vector2(100, 100), Vector2(120, 120));
  startParent->Initialize();

  auto startIcon = std::make_unique<UIImageElement>(
      "title_startIcon", Vector2(0, 0), Vector2(100, 100));
  startIcon->Initialize();
  startButton->AddChild(std::move(startIcon));

  auto optionButton = std::make_unique<UIButtonElement>(
      "title_options", Vector2(100, 250), Vector2(100, 100), "", true);
  optionButton->Initialize();
  optionButton->SetOnHoverEnter([this]() {
    EnterButtonExpandAnimation(
        animationUIElements_[TitleUIElement::OptionsParent]);
  });
  optionButton->SetOnHoverExit([this]() {
    ExitButtonExpandAnimation(
        animationUIElements_[TitleUIElement::OptionsParent]);
  });
  optionButton->SetOnFocusEnter([this]() {
    EnterButtonExpandAnimation(
        animationUIElements_[TitleUIElement::OptionsParent]);
  });
  optionButton->SetOnFocusExit([this]() {
    ExitButtonExpandAnimation(
        animationUIElements_[TitleUIElement::OptionsParent]);
  });
  optionButton->SetOnClickUp([this]() {
    DisPatchEvent("OpenOptionMenu");
    backgroundElement_->SetEnabled(false);
  });
  optionButton->SetOnClick([this]() {
    DisPatchEvent("OpenOptionMenu");
    backgroundElement_->SetEnabled(false);
  });

  auto optionParent = std::make_unique<UIImageElement>(
      "title_optionsParent", Vector2(100, 250), Vector2(120, 120));
  optionParent->Initialize();

  auto optionIcon = std::make_unique<UIImageElement>(
      "title_optionsIcon", Vector2(0, 0), Vector2(100, 100));
  optionIcon->Initialize();
  optionButton->AddChild(std::move(optionIcon));

  auto exitButton = std::make_unique<UIButtonElement>(
      "title_exit", Vector2(100, 400), Vector2(100, 100), "", true);
  exitButton->Initialize();
  exitButton->SetOnHoverEnter([this]() {
    EnterButtonExpandAnimation(
        animationUIElements_[TitleUIElement::ExitParent]);
  });
  exitButton->SetOnHoverExit([this]() {
    ExitButtonExpandAnimation(animationUIElements_[TitleUIElement::ExitParent]);
  });
  exitButton->SetOnFocusEnter([this]() {
    EnterButtonExpandAnimation(
        animationUIElements_[TitleUIElement::ExitParent]);
  });
  exitButton->SetOnFocusExit([this]() {
    ExitButtonExpandAnimation(animationUIElements_[TitleUIElement::ExitParent]);
  });
#ifndef _DEBUG
  exitButton->SetOnClickUp([this]() {
    eventManager_->DispatchEvent(GameEvent("RequestExitGame", nullptr));
  });
  exitButton->SetOnClick([this]() {
    eventManager_->DispatchEvent(GameEvent("RequestExitGame", nullptr));
  });
#endif
  auto exitParent = std::make_unique<UIImageElement>(
      "title_exitParent", Vector2(100, 400), Vector2(120, 120));
  exitParent->Initialize();
  auto exitIcon = std::make_unique<UIImageElement>(
      "title_exitIcon", Vector2(0, 0), Vector2(100, 100));
  exitIcon->Initialize();
  exitButton->AddChild(std::move(exitIcon));

  uiElements_[TitleUIElement::Background] = backgroundElement_.get();
  uiElements_[TitleUIElement::StartButton] =
      startParent->AddChild(std::move(startButton));
  uiElements_[TitleUIElement::OptionsButton] =
      optionParent->AddChild(std::move(optionButton));
  uiElements_[TitleUIElement::ExitButton] =
      exitParent->AddChild(std::move(exitButton));

  auto navi1 = uiElements_[TitleUIElement::StartButton]
                   ->GetComponent<UINavigationComponent>();
  auto navi2 = uiElements_[TitleUIElement::OptionsButton]
                   ->GetComponent<UINavigationComponent>();
  auto navi3 = uiElements_[TitleUIElement::ExitButton]
                   ->GetComponent<UINavigationComponent>();

  navi1->SetNavigation(NavigationDirection::Right,
                       uiElements_[TitleUIElement::OptionsButton]);
  navi1->SetFocused(true); // 最初はスタートボタンにフォーカス
  navi2->SetNavigation(NavigationDirection::Left,
                       uiElements_[TitleUIElement::StartButton]);
  navi2->SetNavigation(NavigationDirection::Right,
                       uiElements_[TitleUIElement::ExitButton]);

  navi3->SetNavigation(NavigationDirection::Left,
                       uiElements_[TitleUIElement::OptionsButton]);

  Vector2 startPos = startParent->GetPosition();
  Vector2 optionPos = optionParent->GetPosition();
  Vector2 exitPos = exitParent->GetPosition();

  animationUIElements_[TitleUIElement::ExitParent] = {
      optionParent->AddChild(std::move(exitParent)), exitPos};
  animationUIElements_[TitleUIElement::OptionsParent] = {
      startParent->AddChild(std::move(optionParent)), optionPos};
  animationUIElements_[TitleUIElement::StartParent] = {
      backgroundElement_->AddChild(std::move(startParent)), startPos};

  auto dummyButton = std::make_unique<UIButtonElement>(
      "title_dummyButton", Vector2(0, 0), Vector2(200, 200), "", true);
  dummyButton->Initialize();
  backgroundElement_->AddChild(std::move(dummyButton));

  backgroundElement_->SetEnabled(false);

  focusFrame_->RegisterTargetName(
      animationUIElements_[TitleUIElement::StartParent].uiElement);
  focusFrame_->RegisterTargetName(
      animationUIElements_[TitleUIElement::OptionsParent].uiElement);
  focusFrame_->RegisterTargetName(
      animationUIElements_[TitleUIElement::ExitParent].uiElement);
}

void TitleUI::UpdateAnimationUI(TitleUIElement elem, float deltaTime) {
  for (size_t i = ToSizeT(elem); i < ToSizeT(TitleUIElement::Max); ++i) {
    TitleUIElement current = ToTitleUIElement(i);

    if (!animationUIElements_.contains(current))
      continue;

    auto &element = animationUIElements_[current];

    if (!element.animating) {
      return;
    }

    element.currentTime += deltaTime;
    if (element.currentTime >= titleAnimationSequence_->GetMaxPlayTime()) {
      element.currentTime = titleAnimationSequence_->GetMaxPlayTime();
      element.animating = false;
    }

    Vector2 val = titleAnimationSequence_->GetValueAtTime<Vector2>(
        element.animationLabel, element.currentTime);
    element.uiElement->SetPosition(element.basePos + val);
  }
}
//
// void TitleUI::UpdateAnimationUI(AnimationUIElement* _element, float
// _deltaTime)
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
//    Vector2 val =
//    titleAnimationSequence_->GetValueAtTime<Vector2>(_element->animationLabel,
//    _element->currentTime); _element->uiElement->SetPos(_element->basePos +
//    val);
//}
