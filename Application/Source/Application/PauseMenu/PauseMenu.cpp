#include "PauseMenu.h"

#include <System/Input/Input.h>
#include <Debug/ImGuiDebugManager.h>


#include <Features/UI/UISprite.h>


void PauseMenu::Initialize()
{

    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    auto sprite = uiGroup_->CreateSprite("PauseMenu_back", L"Pause");

    auto resumeButton = uiGroup_->CreateButton("PauseMenu_ResumeButton", L"Resume");
    resumeButton->SetParent(sprite);

    auto retryButton = uiGroup_->CreateButton("PauseMenu_RetryButton",L"Retry");
    retryButton->SetParent(sprite);

    auto toTitleButton = uiGroup_->CreateButton("PauseMenu_ToTitleButton", L"Title");
    toTitleButton->SetParent(sprite);

    UIGroup::LinkHorizontal(
        { resumeButton, retryButton, toTitleButton }
    );

    sprite_= sprite;

    buttons_["PauseMenu_ResumeButton"] = resumeButton;
    buttons_["PauseMenu_RetryButton"] = retryButton;
    buttons_["PauseMenu_ToTitleButton"] = toTitleButton;
}

void PauseMenu::Update()
{
    if (Input::GetInstance()->IsKeyTriggered(DIK_ESCAPE))
    {
        isActive_ = !isActive_;
        actions_ = isActive_ ? PauseActions::Open : PauseActions::Close; // ポーズメニューの開閉をトグル

    }

    if (!isActive_)
        return;

    uiGroup_->Update();


#ifdef _DEBUG

    if (ImGuiDebugManager::GetInstance()->Begin("pauseMenu"))
    {
        if (ImGui::CollapsingHeader("sprite"))
            sprite_->ImGui();

        for (auto& [key, button] : buttons_)
        {
            if (ImGui::CollapsingHeader(key.c_str()))
            {
                button->ImGui();
            }
        }

        ImGui::End();
    }


#endif

}

void PauseMenu::Draw()
{
    if (!isActive_)
        return;

    uiGroup_->Draw();
}

void PauseMenu::SetCallBacks(const std::function<void()>& _onResumeCallback, const std::function<void()>& _onRetryCallback, const std::function<void()>& _onToTitleCallback)
{
    SetOnResumeCallback(_onResumeCallback);
    SetOnRetryCallback(_onRetryCallback);
    SetOnToTitleCallback(_onToTitleCallback);
}

void PauseMenu::SetOnResumeCallback(const std::function<void()>& _callback)
{
    if (_callback)
    {
        onResumeCallback_ = _callback;

        auto it = buttons_.find("PauseMenu_ResumeButton");
        if (it != buttons_.end())
        {
            it->second->SetCallBackOnClickEnd([this]() {
                isActive_ = false; // ポーズメニューを閉じる
                actions_ = PauseActions::Resume; // 選択されたボタンを更新
                onResumeCallback_(); // コールバックを呼び出す
                });
        }
    }
}

void PauseMenu::SetOnRetryCallback(const std::function<void()>& _callback)
{
    if (_callback)
    {
        onRetryCallback_ = _callback;
        auto it = buttons_.find("PauseMenu_RetryButton");
        if (it != buttons_.end())
        {
            it->second->SetCallBackOnClickEnd([this]() {
                isActive_ = false; // ポーズメニューを閉じる
                actions_ = PauseActions::Retry; // 選択されたボタンを更新
                onRetryCallback_(); // コールバックを呼び出す
                });
        }
    }
}

void PauseMenu::SetOnToTitleCallback(const std::function<void()>& _callback)
{
    if (_callback)
    {
        onToTitleCallback_ = _callback;
        auto it = buttons_.find("PauseMenu_ToTitleButton");
        if (it != buttons_.end())
        {
            it->second->SetCallBackOnClickEnd([this]() {
                isActive_ = false; // ポーズメニューを閉じる
                actions_ = PauseActions::ToTitle; // 選択されたボタンを更新
                onToTitleCallback_(); // コールバックを呼び出す
                });
        }
    }
}
