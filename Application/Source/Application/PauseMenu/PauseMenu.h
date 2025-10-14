#pragma once

#include <Features/UI/UIGroup.h>

//#include <Application/EventData/PauseActionData.h>
#include <Application/Setting/SettingMenu.h>

#include <memory>


class PauseMenu
{
public:

    PauseMenu();
    ~PauseMenu();


    void Initialize();

    void Update();

    void Draw();

    // ポーズメニューがアクティブならゲームは止まっている
    //
    bool IsActive() const { return isActive_; }

private:

    // 有効か否か
    bool isActive_ = false;

    bool isDraw_ = false;

    std::unique_ptr<UIGroup> uiGroup_ = nullptr;

    std::map<std::string, std::shared_ptr<UIButton>> buttons_;

};