#pragma once

#include <Application/Setting/SettingMenu.h>

#include <memory>

/// <summary>
/// オプションメニュー
/// ひとつのシーンの上に重ねて表示する
/// </summary>
class OptionMenu
{
public:

    OptionMenu() = default;
    ~OptionMenu() = default;

    void Initialize();
    void Update();
    void Draw();


private:

    bool isActive_ = false;

    std::unique_ptr<SettingMenu> settingMenu_ = nullptr; // 設定メニュー

    std::shared_ptr<UIButton> closeButton_ = nullptr; // 閉じるボタン
};