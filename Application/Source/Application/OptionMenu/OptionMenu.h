#pragma once

#include <Application/Setting/SettingMenu.h>
#include <Features/UI/Element/UIButtonElement.h>

#include <memory>

/// <summary>
/// オプションメニュー。
/// </summary>
class OptionMenu
{
public:

    OptionMenu() = default;
    ~OptionMenu() = default;

    /// <summary>
    /// 初期化処理。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理。
    /// </summary>
    void Draw();


private:

    bool isActive_ = false;

    std::unique_ptr<SettingMenu> settingMenu_ = nullptr; // 設定メニュー

    std::unique_ptr<Engine::UIButtonElement> closeButton_ = nullptr; // 閉じるボタン
};