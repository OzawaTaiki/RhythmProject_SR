#include "TitleScene.h"
#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Event/EventManager.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <System/Audio/AudioSystem.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Features/UI/Collider/UICollisionManager.h>

TitleScene::TitleScene()
{
    EventManager::GetInstance()->AddEventListener("RequestStartGame", this);
    EventManager::GetInstance()->AddEventListener("RequestExitGame", this);
}

TitleScene::~TitleScene()
{
    EventManager::GetInstance()->RemoveEventListener("RequestStartGame", this);
    EventManager::GetInstance()->RemoveEventListener("RequestExitGame", this);
}

void TitleScene::Initialize(SceneData* _sceneData)
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

    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/Music/Luminous_memory.wav");
    voiceInstance_ = soundInstance_->Play(0.5f, false);

    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    textGenerator_.Initialize(FontConfig(Vector2(1024, 1024), 64));

    LayerSystem::CreateLayer("buttons", 0);
    LayerSystem::CreateLayer("ring", 10);
    LayerSystem::CreateLayer("option", 20);


    // ビートマネージャーの初期化（検出されたBPMを使用）
    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100.0f);
    beatManager_->SetMusicVoiceInstance(voiceInstance_);

    spectrumRing_ = std::make_unique<SpectrumRing>();
    spectrumRing_->Initialize(soundInstance_, 5);
    spectrumRing_->SetBeatManager(beatManager_.get());

    titleUI_ = std::make_unique<TitileUI>();
    titleUI_->Initialize();
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

    particleSystem_->Update();
    settingMenu_->Update();
    titleUI_->Update();

    if (voiceInstance_) // 楽曲が再生中なら楽曲の経過時間を渡す
        spectrumRing_->Update(voiceInstance_->GetElapsedTime());
    else //そうじゃないときは0
        spectrumRing_->Update(0.0f);

    UICollisionManager::GetInstance()->CheckCollision(input_->GetMousePosition());


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
    ModelManager::GetInstance()->PreDrawForObjectModel();

    LayerSystem::SetLayer("ring");
    spectrumRing_->Draw(&SceneCamera_);

    LayerSystem::SetLayer("buttons");

    titleUI_->Draw();
    //textGenerator_.Draw(L"音ゲー", Vector2(640, 200));
    //textGenerator_.Draw(L"Press Enter", Vector2(640, 500));

    LayerSystem::SetLayer("option");
    settingMenu_->Draw();
}

void TitleScene::DrawShadow(){}

void TitleScene::OnEvent(const GameEvent& _event)
{
    if (_event.GetEventType() == "RequestStartGame")
    {
        SceneManager::ReserveScene("GameScene", nullptr);
    }
    else if (_event.GetEventType() == "RequestExitGame")
    {
        PostQuitMessage(0);  // Windows APIでアプリ終了
    }

}
