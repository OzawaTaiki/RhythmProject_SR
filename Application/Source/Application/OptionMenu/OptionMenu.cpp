#include "OptionMenu.h"

#include <Features/Event/EventManager.h>
#include <Debug/Debug.h>

void OptionMenu::Initialize()
{
    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    // 閉じるボタンの初期化
    closeButton_ = std::make_shared<UIButton>();
    closeButton_->Initialize("CloseButton", L"Close");
    closeButton_->SetOnClick([&]()
                             {
                                 isActive_ = false;
                                 Debug::Log("OptionMenu closed\n");
                             });
    isActive_ = false;
}

void OptionMenu::Update()
{
    if (!isActive_)
        return;

    closeButton_->Update();
    settingMenu_->Update();
}

void OptionMenu::Draw()
{
    if (!isActive_)
        return;

    settingMenu_->Draw();
    closeButton_->Draw();
}
