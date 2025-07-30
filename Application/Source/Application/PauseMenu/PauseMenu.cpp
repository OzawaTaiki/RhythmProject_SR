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
    resumeButton->SetCallBackOnClickEnd([this]() {
        isActive_ = false; // ポーズメニューを閉じる
        selectedButton_ = PauseMenuButton::Resume; // 選択されたボタンを更新
        });
    resumeButton->SetParent(sprite);
    auto retryButton = uiGroup_->CreateButton("PauseMenu_RetryButton",L"Retry");
    retryButton->SetCallBackOnClickEnd([this]() {
        isActive_ = false; // ポーズメニューを閉じる
        selectedButton_ = PauseMenuButton::Retry; // 選択されたボタンを更新
        });
    retryButton->SetParent(sprite);

    auto toTitleButton = uiGroup_->CreateButton("PauseMenu_ToTitleButton", L"Title");
    toTitleButton->SetCallBackOnClickEnd([this]() {
        isActive_ = false; // ポーズメニューを閉じる
        selectedButton_ = PauseMenuButton::ToTitile; // 選択されたボタンを更新
        });
    toTitleButton->SetParent(sprite);

    UIGroup::LinkHorizontal(
        { resumeButton, retryButton, toTitleButton }
    );

#ifdef _DEBUG
    debugSprite_ = sprite;

    debugButton_.push_back(resumeButton);
    debugButton_.push_back(retryButton);
    debugButton_.push_back(toTitleButton);

#endif

}

void PauseMenu::Update()
{
    if (Input::GetInstance()->IsKeyTriggered(DIK_ESCAPE))
        isActive_ = !isActive_;

    if (!isActive_)
        return;

    uiGroup_->Update();


#ifdef _DEBUG

    if (ImGuiDebugManager::GetInstance()->Begin("pauseMenu"))
    {
        std::string strings[3] = { "resume", "retry", "toTitle" };
        if (ImGui::CollapsingHeader("sprite"))
            debugSprite_->ImGui();

        for (size_t i = 0; i < 3; i++)
        {
            if (ImGui::CollapsingHeader(strings[i].c_str()))
            {
                debugButton_[i]->ImGui();
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
