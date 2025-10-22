#pragma once

#include <Features/UI/UIGroup.h>

//#include <Application/EventData/PauseActionData.h>
#include <Application/Setting/SettingMenu.h>

#include <memory>

// ポーズメニュー
class PauseMenu
{
public:

    PauseMenu();
    ~PauseMenu();

    // 初期化
    void Initialize();
    // 更新
    void Update();
    // 描画
    void Draw();

    // ポーズメニューがアクティブならゲームは止まっている
    // アクティブか否か
    bool IsActive() const { return isActive_; }

private:

    bool isActive_ = false; // 有効フラグ

    bool isDraw_ = false; // 描画フラグ

    std::unique_ptr<UIGroup> uiGroup_ = nullptr; // UIグループ

    std::map<std::string, std::shared_ptr<UIButton>> buttons_; // ボタン群

};