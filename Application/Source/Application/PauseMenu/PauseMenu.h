#pragma once

#include <Features/UI/UIGroup.h>

#include <memory>

enum class PauseMenuButton
{
    None,
    Resume,
    Retry,
    ToTitile,

    Max
    // memo : titileは仮 セレクト実装するまで

};

class PauseMenu
{
public:

    void Initialize();

    void Update();

    void Draw();

    // ポーズメニューがアクティブならゲームは止まっている
    //
    bool IsActive() const { return isActive_; }

    PauseMenuButton GetSelectedButton() const { return selectedButton_; }

private:

    // 有効か否か
    bool isActive_ = false;

    PauseMenuButton selectedButton_ = PauseMenuButton::None;

    std::unique_ptr<UIGroup> uiGroup_ = nullptr;

#ifdef _DEBUG

    std::vector<UIButton*> debugButton_;
    UISprite* debugSprite_ = nullptr;

#endif

};