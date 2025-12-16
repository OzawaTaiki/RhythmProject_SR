#include "TitleScene.h"
#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Event/EventManager.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <System/Audio/AudioSystem.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Features/UI/Collider/UICollisionManager.h>
#include <Constants/MathConstants.h>
#include <Features/UI/UINavigationManager.h>

TitleScene::TitleScene()
{
    EventManager::GetInstance()->AddEventListener("RequestStartGame", this);
    EventManager::GetInstance()->AddEventListener("RequestExitGame", this);
}

TitleScene::~TitleScene()
{
    EventManager::GetInstance()->RemoveEventListener("RequestStartGame", this);
    EventManager::GetInstance()->RemoveEventListener("RequestExitGame", this);

    UINavigationManager::GetInstance()->ClearFocus();
}

void TitleScene::Initialize([[maybe_unused]] SceneData* sceneData)
{
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,0,-10 };
    SceneCamera_.rotate_ = { 0.0f,0,0 };
    SceneCamera_.UpdateMatrix();
    debugCamera_.Initialize();

    camera2d_.Initialize(CameraType::Orthographic);

    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->Initialize();
    lineDrawer_->SetCameraPtr(&SceneCamera_);
    lineDrawer_->SetCameraPtr2D(&camera2d_);

    input_ = Input::GetInstance();

    particleSystem_ = ParticleSystem::GetInstance();
    particleSystem_->SetCamera(&SceneCamera_);

    lightGroup_ = std::make_shared<LightGroup>();
    lightGroup_->Initialize();


    LightingSystem::GetInstance()->SetActiveGroup(lightGroup_);

    ///
    ///
    ///

    titleCamera_.Initialize();

    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/Music/demoMusic.wav");
    voiceInstance_ = soundInstance_->Play(0.5f, false);

    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    textGenerator_.Initialize(FontConfig(Vector2(1024, 1024), 64));

    LayerSystem::CreateLayer("back", 0);
    LayerSystem::CreateLayer("buttons", 20);
    LayerSystem::CreateLayer("ring", 40);
    LayerSystem::CreateLayer("option", 60);


    // ビートマネージャーの初期化
    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100.0f);
    beatManager_->SetMusicVoiceInstance(voiceInstance_);

    spectrumRing_ = std::make_unique<SpectrumRing>();
    spectrumRing_->Initialize(soundInstance_, 5);
    spectrumRing_->SetBeatManager(beatManager_.get());

    titleUI_ = std::make_unique<TitleUI>();
    titleUI_->Initialize();

    titleBack_ = std::make_unique<UIImageElement>("Title_Background", WinApp::kWindowSize_ * 0.5f, WinApp::kWindowSize_);
    titleBack_->Initialize();
    UVTransform& uvTransform = titleBack_->GetUVTransform();
    uvTransform.SetRotation(MathConstants::kHalfPi / 3.0f);
    uvTransform.SetScale(Vector2(10.0f, 10.0f));
    uvAnimation_.AddTransform(&uvTransform);
    uvAnimation_.SetLooping(true);
    uvAnimation_.SetScrollSpeed(Vector2(0.2f, -0.1f));
    uvAnimation_.Play();

    hexagonGrid_ = std::make_unique<HexagonGrid>();
    Rect area(Vector2(0, 0), Vector2(1280, 720));
    hexagonGrid_->Initialize(area);
}

void TitleScene::Update()
{
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    lightGroup_->ImGui();

#endif // _DEBUG

    if (!voiceInstance_ || !voiceInstance_->IsPlaying())
    {
        // 楽曲が終了したら最初から再生
        voiceInstance_ = soundInstance_->Play(0.5f, false);
        beatManager_->SetMusicVoiceInstance(voiceInstance_);
        // 譜面フォルダからランダムで曲を流したい
    }

    uvAnimation_.Update(Time::GetDeltaTime<float>());

    particleSystem_->Update();
    settingMenu_->Update();
    titleUI_->Update();
#ifndef _DEBUG
    hexagonGrid_->Update();
#endif
    titleBack_->Update();
    if (voiceInstance_) // 楽曲が再生中なら楽曲の経過時間を渡す
        spectrumRing_->Update(voiceInstance_->GetElapsedTime());
    else //そうじゃないときは0
        spectrumRing_->Update(0.0f);

    if (input_->IsKeyPressed(DIK_LCONTROL) &&
        input_->IsKeyTriggered(DIK_O))
    {
        EventManager::GetInstance()->DispatchEvent(GameEvent("OpenOptionMenu", nullptr));
    }
    if (input_->IsKeyTriggered(DIK_F10) &&
        input_->IsKeyPressed(DIK_F1))
    {
        SceneManager::ReserveScene("Sample", nullptr);
    }

    if (enableDebugCamera_)
    {
        debugCamera_.Update();
        SceneCamera_.matView_ = debugCamera_.matView_;
        SceneCamera_.TransferData();
    }
    else
    {
        titleCamera_.Update(Time::GetDeltaTime<float>());
        SceneCamera_ = *titleCamera_.GetCamera();
        SceneCamera_.UpdateMatrix();
    }

}

void TitleScene::Draw()
{
    LayerSystem::SetLayer("back");
    {
    }

    ModelManager::GetInstance()->PreDrawForObjectModel();

    LayerSystem::SetLayer("back");
    {
        titleBack_->Draw();
        hexagonGrid_->Draw();
    }

    LayerSystem::SetLayer("ring");
    {
        spectrumRing_->Draw(&SceneCamera_);
    }

    LayerSystem::SetLayer("buttons");
    {
        titleUI_->Draw();
    }

    LayerSystem::SetLayer("option");
    {
        settingMenu_->Draw();
    }
}

void TitleScene::DrawShadow() {}

void TitleScene::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "RequestStartGame")
    {
        SceneManager::ReserveScene("GameScene", nullptr);
    }
    else if (event.GetEventType() == "RequestExitGame")
    {
        PostQuitMessage(0);  // Windows APIでアプリ終了
    }

}
