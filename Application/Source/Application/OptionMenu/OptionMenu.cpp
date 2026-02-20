#include "OptionMenu.h"

#include <Features/Event/EventManager.h>
#include <Debug/Debug.h>

using namespace Engine;

void OptionMenu::Initialize()
{
    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    // 髢峨§繧九・繧ｿ繝ｳ縺ｮ蛻晄悄蛹・
    closeButton_ = std::make_unique<UIButtonElement>("option_closeButton", Vector2(700, 50), Vector2(80, 30),"Close");
    closeButton_->Initialize();
    closeButton_->SetOnClickUp([&]()
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
