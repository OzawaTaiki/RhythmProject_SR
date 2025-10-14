#include "PauseMenu.h"

#include <System/Input/Input.h>
#include <Debug/ImGuiDebugManager.h>
#include <Features/Event/EventManager.h>

#include <Features/UI/UISprite.h>
#include <Features/UI/UIButton.h>
#include <Features/UI/UISlider.h>
#include <Debug/Debug.h>
#include <Application/Setting/Setting.h>

#include <System/Audio/AudioSystem.h>



PauseMenu::PauseMenu()
{
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::Initialize()
{
    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    auto backSprite = uiGroup_->CreateSprite("PauseMenu_blackback");
    backSprite->SetSize({ 1280, 720 });
    backSprite->SetColor({ 0, 0, 0, 0.8f }); // 半透明の黒背景
    backSprite->SetPos({ 0, 0 });
    backSprite->SetAnchor({ 0,0 });// 左上にアンカーを設定

    auto sprite = uiGroup_->CreateSprite("PauseMenu_back", L"Pause");

    auto resumeButton   = uiGroup_->CreateButton("PauseMenu_ResumeButton",  L"Resume");
    {
        resumeButton->SetOnClickEnd([&]()
                                    {
                                        EventManager::GetInstance()->DispatchEvent(GameEvent("RequestResume", nullptr));
                                        isActive_ = false;
                                        isDraw_ = false;
                                        Debug::Log("Resume button clicked\n");
                                    });
    }
    auto retryButton    = uiGroup_->CreateButton("PauseMenu_RetryButton",   L"Retry");
    {
        retryButton->SetOnClickEnd([&]()
                                   {
                                       EventManager::GetInstance()->DispatchEvent(GameEvent("RequestRetry", nullptr));
                                       isActive_ = false;
                                       isDraw_ = false;
                                       Debug::Log("Retry button clicked\n");
                                   });
    }
    auto toTitleButton  = uiGroup_->CreateButton("PauseMenu_ToTitleButton", L"Title");
    {
        toTitleButton->SetOnClickEnd([&]()
                                     {
                                         EventManager::GetInstance()->DispatchEvent(GameEvent("RequestToTitle", nullptr));
                                         isActive_ = false;
                                         isDraw_ = false;
                                         Debug::Log("ToTitle button clicked\n");
                                     });
    }
    auto toOptionButton = uiGroup_->CreateButton("PauseMenu_OptionButton",  L"Option");
    {
        auto optionButtonBack = std::make_shared<UISprite>();
        optionButtonBack->Initialize("PauseMenu_OptionButtonIcon");
        toOptionButton->AddChild(optionButtonBack);

        toOptionButton->SetOnClickEnd([&]()
                                      {
                                          EventManager::GetInstance()->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));
                                          isDraw_ = false;
                                          Debug::Log("Option button clicked\n");
                                      });

    }

    sprite->AddChild(resumeButton);
    sprite->AddChild(retryButton);
    sprite->AddChild(toTitleButton);
    sprite->AddChild(toOptionButton);

    UIGroup::LinkHorizontal(
        { resumeButton.get(), retryButton.get(), toTitleButton.get() }
    );

    // 下押してオプションへ
    toOptionButton->SetNavigationTarget(resumeButton.get(), Direction::Down);
    toOptionButton->SetNavigationTarget(retryButton.get(), Direction::Down);
    toOptionButton->SetNavigationTarget(toTitleButton.get(), Direction::Down);
}

void PauseMenu::Update()
{
    if (!isActive_ || !isDraw_)
    {
        if (Input::GetInstance()->IsKeyTriggered(DIK_ESCAPE))
        {
            isActive_ = true;
            isDraw_ = true;
        }
        return;
    }

    uiGroup_->Update();
}

void PauseMenu::Draw()
{
    if (!isDraw_)
        return;

    uiGroup_->Draw();

}
