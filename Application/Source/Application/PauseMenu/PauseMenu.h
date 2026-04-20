#pragma once

//#include <Application/EventData/PauseActionData.h>
#include <Application/Setting/SettingMenu.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/Event/EventListener.h>

#include <memory>

namespace Engine { class UIAnimationComponent; }

/// <summary>
/// ポーズメニュークラス。
/// </summary>
class PauseMenu : public Engine::iEventListener
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
    /// イベントを受信するハンドラ。
    /// </summary>
    /// <param name="event"> 受信したイベント</param>
    void OnEvent(const Engine::GameEvent& event) override;

    /// <summary>
    /// ポーズメニューがアクティブかどうかを返す。
    /// </summary>
    bool IsActive() const { return isActive_; }

    void ToActive();
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

    bool isActive_ = false;
    bool isDraw_ = false;

    Engine::UIElement* resumeButton_ = nullptr;
    std::unique_ptr<Engine::UIImageElement> background_ = nullptr;

    Engine::UIAnimationComponent* openAnim_ = nullptr;
    Engine::UIAnimationComponent* closeAnim_ = nullptr;
};
