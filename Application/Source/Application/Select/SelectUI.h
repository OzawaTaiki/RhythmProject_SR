#pragma once

#include <Features/UI/Element/UIButtonElement.h>
#include <Features/Event/EventListener.h>

#include <Application/Setting/SettingMenu.h>

#include "musicSelectUI.h"

#include <memory>

/// <summary>
/// 選曲画面のUIクラス
/// </summary>
// 選曲UI，設定ボタン，楽曲スピードのUIなどを管理
class SelectUI : iEventListener
{
public:
    SelectUI();
    ~SelectUI();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update(float deltaTime);

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();

    void OnEvent(const GameEvent& event) override;

private:

    std::unique_ptr<MusicSelectUI> musicSelectUI_;

    std::unique_ptr<UIButtonElement> settingButton_; // 設定ボタン

    bool isSettingMenuActive_ = false; // 設定メニューがアクティブかどうか
    std::unique_ptr<SettingMenu> settingMenu_; // 設定メニュー

};

