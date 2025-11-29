#include "PauseMenu.h"

#include <System/Input/Input.h>
#include <Debug/ImGuiDebugManager.h>
#include <Features/Event/EventManager.h>

#include <Features/UI/UISprite.h>
#include <Features/UI/UIButton.h>
#include <Features/UI/UISlider.h>
#include <Debug/Debug.h>
#include <Application/Setting/Setting.h>

#include <System/Audio/AudioSystem.h>



PauseMenu::PauseMenu()
{
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::Initialize()
{
    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    auto backSprite = uiGroup_->CreateSprite("PauseMenu_blackback");
    backSprite->SetSize(WinApp::kWindowSize_);
    backSprite->SetColor({ 0, 0, 0, 0.8f }); // 半透明の黒背景
    backSprite->SetPos({ 0, 0 });
    backSprite->SetAnchor({ 0,0 });// 左上にアンカーを設定

    auto sprite = uiGroup_->CreateSprite("PauseMenu_back", L"Pause");

    // 再開ボタンの初期化
    auto resumeButton   = uiGroup_->CreateButton("PauseMenu_ResumeButton",  L"Resume");
    {
        resumeButton->SetOnClickEnd([&]()
                                    {
                                        EventManager::GetInstance()->DispatchEvent(GameEvent("RequestResume", nullptr));// ポーズ解除イベントを送信
                                        isActive_ = false; // ポーズメニューを非アクティブに
                                        isDraw_ = false; // 描画を停止
                                        Debug::Log("Resume button clicked\n");
                                    });
    }
    // リトライボタンの初期化
    auto retryButton    = uiGroup_->CreateButton("PauseMenu_RetryButton",   L"Retry");
    {
        retryButton->SetOnClickEnd([&]()
                                   {
                                       EventManager::GetInstance()->DispatchEvent(GameEvent("RequestRetry", nullptr));// リトライイベントを送信
                                       isActive_ = false;// ポーズメニューを非アクティブに
                                       isDraw_ = false;// 描画を停止
                                       Debug::Log("Retry button clicked\n");
                                   });
    }
    // タイトルへ戻るボタンの初期化
    auto toTitleButton  = uiGroup_->CreateButton("PauseMenu_ToTitleButton", L"Title");
    {
        toTitleButton->SetOnClickEnd([&]()
                                     {
                                         EventManager::GetInstance()->DispatchEvent(GameEvent("RequestToTitle", nullptr));// タイトルへ戻るイベントを送信
                                         isActive_ = false; // ポーズメニューを非アクティブに
                                         isDraw_ = false;// 描画を停止
                                         Debug::Log("ToTitle button clicked\n");
                                     });
    }
    // オプションボタンの初期化
    auto toOptionButton = uiGroup_->CreateButton("PauseMenu_OptionButton",  L"Option");
    {
        // オプションアイコン用のスプライトの初期化
        auto optionButtonBack = std::make_shared<UISprite>();
        optionButtonBack->Initialize("PauseMenu_OptionButtonIcon");
        toOptionButton->AddChild(optionButtonBack);

        toOptionButton->SetOnClickEnd([&]()
                                      {
                                          // オプションメニューを開くイベントを送信
                                          EventManager::GetInstance()->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));
                                          isDraw_ = false;
                                          Debug::Log("Option button clicked\n");
                                      });

    }

    // ボタンをポーズメニュースプライトの子として追加
    sprite->AddChild(resumeButton);
    sprite->AddChild(retryButton);
    sprite->AddChild(toTitleButton);
    sprite->AddChild(toOptionButton);

    UIGroup::LinkHorizontal(
        { resumeButton.get(), retryButton.get(), toTitleButton.get() }
    );

    // ナビゲーション設定
    toOptionButton->SetNavigationTarget(resumeButton.get(), Direction::Down);
    toOptionButton->SetNavigationTarget(retryButton.get(), Direction::Down);
    toOptionButton->SetNavigationTarget(toTitleButton.get(), Direction::Down);
}

void PauseMenu::Update()
{
    if (!isActive_ || !isDraw_)
    {
        if (Input::GetInstance()->IsKeyTriggered(DIK_ESCAPE))// ESCキーでポーズメニューを開く
        {
            isActive_ = true;
            isDraw_ = true;
        }
        return;
    }

    uiGroup_->Update();
}

void PauseMenu::Draw()
{
    if (!isDraw_)
        return;

    uiGroup_->Draw();

}
