#include "TitleScene.h"
#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Event/EventManager.h>
#include <System/Audio/AudioSystem.h>
#include <Features/Model/Manager/ModelManager.h>

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

    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/Music/Luminous_memory.wav");
    voiceInstance_ = soundInstance_->Play(0.5f, true);

    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    textGenerator_.Initialize(FontConfig(Vector2(1024, 1024), 64));

    spectrumRing_ = std::make_unique<SpectrumRing>();
    spectrumRing_->Initialize(soundInstance_, 5);

}

void TitleScene::Update()
{
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    lightGroup_->ImGui();

#endif // _DEBUG

    if (voiceInstance_) // 楽曲が再生中なら楽曲の経過時間を渡す
        spectrumRing_->Update(voiceInstance_->GetElapsedTime());
    else //そうじゃないときは0
        spectrumRing_->Update(0.0f);



    if (input_->IsKeyPressed(DIK_LCONTROL) &&
        input_->IsKeyTriggered(DIK_O))
    {
        EventManager::GetInstance()->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));
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

    particleSystem_->Update();
    settingMenu_->Update();

}

void TitleScene::Draw()
{
    ModelManager::GetInstance()->PreDrawForObjectModel();

    spectrumRing_->Draw(&SceneCamera_);


    settingMenu_->Draw();
    textGenerator_.Draw(L"音ゲー", Vector2(640, 200));
    textGenerator_.Draw(L"Press Enter", Vector2(640, 500));
}

void TitleScene::DrawShadow(){}
