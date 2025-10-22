#include "TitleScene.h"
#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Event/EventManager.h>
#include <Framework/LayerSystem/LayerSystem.h>

void TitleScene::Initialize(SceneData* _sceneData)
{
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,5,-13 };
    SceneCamera_.rotate_ = { 0.26f,0,0 };
    SceneCamera_.UpdateMatrix();
    debugCamera_.Initialize();


    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->Initialize();
    lineDrawer_->SetCameraPtr(&SceneCamera_);

    input_ = Input::GetInstance();

    particleSystem_ = ParticleSystem::GetInstance();
    particleSystem_->SetCamera(&SceneCamera_);

    lightGroup_ = std::make_shared<LightGroup>();
    lightGroup_->Initialize();


    LightingSystem::GetInstance()->SetActiveGroup(lightGroup_);

    ///
    ///
    ///

    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    textGenerator_.Initialize(FontConfig(Vector2(1024, 1024), 64));

    LayerSystem::CreateLayer("main", 0);
    LayerSystem::CreateLayer("option", 1);

}

void TitleScene::Update()
{
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    lightGroup_->ImGui();

#endif // _DEBUG

    particleSystem_->Update();
    settingMenu_->Update();

    if (input_->IsKeyTriggered(DIK_ESCAPE))
    {
        EventManager::GetInstance()->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));
    }
    if (input_->IsKeyTriggered(DIK_F10) &&
        input_->IsKeyPressed(DIK_F1))
    {
        SceneManager::ReserveScene("Sample", nullptr);
    }
    if (input_->IsKeyTriggered(DIK_RETURN))
    {
        // シーンの切り替え
        SceneManager::ReserveScene("GameScene", nullptr);
    }

    if (enableDebugCamera_)
    {
        debugCamera_.Update();
        SceneCamera_.matView_ = debugCamera_.matView_;
        SceneCamera_.TransferData();
    }
    else
    {
        SceneCamera_.Update();
        SceneCamera_.UpdateMatrix();
    }


}

void TitleScene::Draw()
{
    LayerSystem::SetLayer("main");
    textGenerator_.Draw(L"音ゲー", Vector2(640, 200));
    textGenerator_.Draw(L"Press Enter", Vector2(640, 500));

    LayerSystem::SetLayer("option");
    settingMenu_->Draw();
}

void TitleScene::DrawShadow(){}
