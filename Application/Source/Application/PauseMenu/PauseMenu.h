#pragma once

#include <Features/UI/UIGroup.h>

//#include <Application/EventData/PauseActionData.h>
#include <Application/Setting/SettingMenu.h>

#include <memory>

/// <summary>
/// ポーズメニュークラス。
/// </summary>
class PauseMenu
{
public:

    PauseMenu();
    ~PauseMenu();

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

    /// <summary>
    /// ポーズメニューがアクティブかどうかを返す。
    /// </summary>
    bool IsActive() const { return isActive_; }

private:

    bool isActive_ = false; // 有効フラグ

    bool isDraw_ = false; // 描画フラグ

    std::unique_ptr<UIGroup> uiGroup_ = nullptr; // UIグループ

    std::map<std::string, std::shared_ptr<UIButton>> buttons_; // ボタン群

};