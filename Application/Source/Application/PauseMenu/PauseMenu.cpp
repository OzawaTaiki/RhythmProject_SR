#include "PauseMenu.h"

#include <System/Input/Input.h>
#include <Debug/ImGuiDebugManager.h>
#include <Features/Event/EventManager.h>
#include <Features/UI/Component/UITextRenderComponent.h>

#include <Debug/Debug.h>
#include <Application/Setting/Setting.h>


#include <System/Audio/AudioSystem.h>
#include <Features/UI/UINavigationManager.h>



PauseMenu::PauseMenu()
{
    EventManager::GetInstance()->AddEventListener("CloseOptionMenu", this);
    EventManager::GetInstance()->AddEventListener("OpenPauseMenu", this);
}

PauseMenu::~PauseMenu()
{
    EventManager::GetInstance()->RemoveEventListener("CloseOptionMenu", this);
    EventManager::GetInstance()->RemoveEventListener("OpenPauseMenu", this);
}

void PauseMenu::Initialize()
{
    background_ = std::make_unique<UIImageElement>("PauseMenu_background", WinApp::kWindowSize_, WinApp::kWindowSize_);
    background_->Initialize();

    auto sprite = std::make_unique<UIImageElement>("PauseMenu_back", Vector2(400, 300), Vector2(400, 300));
    sprite->Initialize();
    sprite->AddComponent<UITextRenderComponent>(sprite.get());

    auto resumeButton = std::make_unique<UIButtonElement>("PauseMenu_ResumeButton", Vector2(150, 200), Vector2(100, 50), "Resume");
    resumeButton->Initialize();
    resumeButton->SetOnClickUp([&]() { CliclEvent(EventType::Resume); });
    resumeButton->SetOnClick([&]() { CliclEvent(EventType::Resume); });

    auto retryButton = std::make_unique<UIButtonElement>("PauseMenu_RetryButton", Vector2(150, 270), Vector2(100, 50), "Retry");
    retryButton->Initialize();
    retryButton->SetOnClickUp([&]() { CliclEvent(EventType::Restart); });
    retryButton->SetOnClick([&]() { CliclEvent(EventType::Restart); });

    auto toTitleButton = std::make_unique<UIButtonElement>("PauseMenu_ToTitleButton", Vector2(150, 340), Vector2(100, 50), "Title");
    toTitleButton->Initialize();
    toTitleButton->SetOnClickUp([&]() { CliclEvent(EventType::ExitToTitle); });
    toTitleButton->SetOnClick([&]() { CliclEvent(EventType::ExitToTitle); });

    auto toOptionButton = std::make_unique<UIButtonElement>("PauseMenu_OptionButton", Vector2(150, 410), Vector2(100, 50), "Option");
    toOptionButton->Initialize();
    toOptionButton->SetOnClickUp([&]() { CliclEvent(EventType::OpenSettings); });
    toOptionButton->SetOnClick([&]() { CliclEvent(EventType::OpenSettings); });

    auto navi1 = resumeButton->GetComponent<UINavigationComponent>();
    auto navi2 = retryButton->GetComponent<UINavigationComponent>();
    auto navi3 = toTitleButton->GetComponent<UINavigationComponent>();
    auto navi4 = toOptionButton->GetComponent<UINavigationComponent>();

    navi1->SetNavigation(NavigationDirection::Right, retryButton.get());
    navi1->SetNavigation(NavigationDirection::Up, toOptionButton.get());

    navi2->SetNavigation(NavigationDirection::Left, resumeButton.get());
    navi2->SetNavigation(NavigationDirection::Right, toTitleButton.get());
    navi2->SetNavigation(NavigationDirection::Up, toOptionButton.get());

    navi3->SetNavigation(NavigationDirection::Left, retryButton.get());
    navi3->SetNavigation(NavigationDirection::Up, toOptionButton.get());

    navi4->SetNavigation(NavigationDirection::Down, resumeButton.get());
    navi4->SetNavigation(NavigationDirection::Down, retryButton.get());
    navi4->SetNavigation(NavigationDirection::Down, toTitleButton.get());

    resumeButton_ = sprite->AddChild(std::move(resumeButton));
    sprite->AddChild(std::move(retryButton));
    sprite->AddChild(std::move(toTitleButton));
    sprite->AddChild(std::move(toOptionButton));
    background_->AddChild(std::move(sprite));

}

void PauseMenu::Update()
{
    if (!isActive_ || !isDraw_)
    {
        if (Input::GetInstance()->IsKeyTriggered(DIK_ESCAPE))
        {
            ToActive();
        }
        return;
    }

    if(UINavigationManager::GetInstance()->GetFocus() == nullptr)
    {
        UINavigationManager::GetInstance()->SetFocus(resumeButton_);
    }
    background_->Update();
}

void PauseMenu::Draw()
{
    if (!isDraw_)
        return;

    background_->Draw();

}

void PauseMenu::OnEvent(const GameEvent& event)
{
    const std::string& eventType = event.GetEventType();
    if (eventType == "CloseOptionMenu")
    {
        ToActive();
    }
}

void PauseMenu::ToActive()
{
    isActive_ = true;
    isDraw_ = true;
}

void PauseMenu::CliclEvent(EventType element)
{
    std::string eventName = GetDispatchEventName(element);
    if (!eventName.empty())
    {
        UINavigationManager::GetInstance()->ClearFocus();
        EventManager::GetInstance()->DispatchEvent(GameEvent(eventName, nullptr));
        isDraw_ = false; // 描画を停止
        if (element != EventType::OpenSettings)
            isActive_ = false; // ポーズメニューを非アクティブに
    }
}

std::string PauseMenu::GetDispatchEventName(EventType element)
{
    switch (element)
    {
        case EventType::Resume:
            return "RequestResume";
        case EventType::Restart:
            return "RequestRetry";
        case EventType::ExitToTitle:
            return "RequestToTitle";
        case EventType::OpenSettings:
            return "OpenOptionMenu";
        default:
            return "";
    }
}
