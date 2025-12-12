#pragma once

#include <Features/UI/UIGroup.h>

//#include <Application/EventData/PauseActionData.h>
#include <Application/Setting/SettingMenu.h>
#include <Features/UI/UIImageElement.h>

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

    enum class EventType
    {
        Resume,
        Restart,
        ExitToTitle,
        OpenSettings,
        Max
    };
    void CliclEvent(EventType element);

    std::string GetDispatchEventName(EventType element);

private:

    bool isActive_ = false; // 有効フラグ

    bool isDraw_ = false; // 描画フラグ

    UIElement* resumeButton_ = nullptr; // レジュームボタン フォーカス要
    std::unique_ptr<UIImageElement> background_ = nullptr; // 背景画像
};