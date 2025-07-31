#pragma once

#include <Features/UI/UIGroup.h>

#include <Application/EventData/PauseActionData.h>

#include <memory>

class EventManager;

class PauseMenu
{
public:

    void Initialize();

    void Update();

    void Draw();

    // ポーズメニューがアクティブならゲームは止まっている
    //
    bool IsActive() const { return isActive_; }

    PauseActions GetSelectedButton() const { return actions_; }

    void SetCallBacks(const std::function<void()>& _onResumeCallback,
                    const std::function<void()>& _onRetryCallback,
                    const std::function<void()>& _onToTitleCallback);

    void SetOnResumeCallback(const std::function<void()>& _callback);
    void SetOnRetryCallback(const std::function<void()>& _callback)   ;
    void SetOnToTitleCallback(const std::function<void()>& _callback) ;


private:

    // 有効か否か
    bool isActive_ = false;

    PauseActions actions_ = PauseActions::None;

    std::unique_ptr<UIGroup> uiGroup_ = nullptr;

    std::function<void()> onResumeCallback_ = nullptr; // レジュームボタンのコールバック
    std::function<void()> onRetryCallback_ = nullptr;  // リトライボタンのコールバック
    std::function<void()> onToTitleCallback_ = nullptr; // タイトルに戻るボタンのコールバック

    std::map<std::string, UIButton*> buttons_;
    UISprite* sprite_ = nullptr;

};