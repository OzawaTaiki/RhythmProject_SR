#include "SelectUI.h"

#include <Features/UI/Element/UIImageElement.h>
#include <Features/Event/EventManager.h>

SelectUI::SelectUI()
{
    EventManager::GetInstance()->AddEventListener("CloseOptionMenu", this);
}

SelectUI::~SelectUI()
{
    EventManager::GetInstance()->RemoveEventListener("CloseOptionMenu", this);
}

void SelectUI::Initialize()
{
    musicSelectUI_ =  std::make_unique<MusicSelectUI>();
    musicSelectUI_->Initialize();

    settingMenu_ =std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    settingButton_ = std::make_unique<UIButtonElement>("Select_Setting_Button", Vector2(0, 0), Vector2(100, 100));
    settingButton_->Initialize();
    settingButton_->SetOnClickUp([&]()
                                 {
                                     isSettingMenuActive_ = true;
                                     EventManager::GetInstance()->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));

                                 });
    settingButton_->SetOnClick([&]()
                               {
                                   isSettingMenuActive_ = true;
                                     EventManager::GetInstance()->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));
                               });
    settingButton_->SetOnFocusEnter([]()
                                    {
                                        // TODO: フォーカス取得時の処理
                                    });
    settingButton_->SetOnFocusExit([]()
                                   {
                                       // TODO: フォーカス離脱時の処理
                                   });

    auto sprite = std::make_unique<UIImageElement>("Select_Setting_Icon", Vector2(0, 0), Vector2(64, 64), "GearIcon.png");
    sprite->Initialize();
    settingButton_->AddChild(std::move(sprite));
}

void SelectUI::Update(float deltaTime)
{
    musicSelectUI_->Update(deltaTime);
    settingButton_->Update();

    if (isSettingMenuActive_)
        settingMenu_->Update();
}

void SelectUI::Draw()
{
    settingButton_->Draw();
    musicSelectUI_->Draw();

    if (isSettingMenuActive_)
    {
        settingMenu_->Draw();
    }
}

void SelectUI::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "CloseOptionMenu")
    {
        isSettingMenuActive_ = false;
    }
}
