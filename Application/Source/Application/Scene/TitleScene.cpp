#include "TitleScene.h"
#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Event/EventManager.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <System/Audio/AudioSystem.h>
#include <Features/Model/Manager/ModelManager.h>

void TitleScene::Initialize(SceneData* _sceneData)
{
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,0,-20 };
    SceneCamera_.rotate_ = { 0.0f,0,0 };
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

    LayerSystem::CreateLayer("main", 0);
    LayerSystem::CreateLayer("option", 1);

    // BPM検出器の初期化
    //bpmDetector_ = std::make_unique<BPMDetector>();
    //bpmDetector_->SetMinBPM(60.0f);
    //bpmDetector_->SetMaxBPM(200.0f);

    // 譜面データを読み込んでBPMを持ってくるがいいかもしれない
    // SoundInstanceからBPMを検出  精度はあまり高くない(要検討)
    //float detectedBPM = bpmDetector_->DetectBPM(soundInstance_.get());

    // ビートマネージャーの初期化（検出されたBPMを使用）
    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100.0f);
    beatManager_->SetMusicVoiceInstance(voiceInstance_);

    spectrumRing_ = std::make_unique<SpectrumRing>();
    spectrumRing_->Initialize(soundInstance_, 5);
    spectrumRing_->SetBeatManager(beatManager_.get());


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
    ModelManager::GetInstance()->PreDrawForObjectModel();

    spectrumRing_->Draw(&SceneCamera_);


    settingMenu_->Draw();
    textGenerator_.Draw(L"音ゲー", Vector2(640, 200));
    textGenerator_.Draw(L"Press Enter", Vector2(640, 500));

    LayerSystem::SetLayer("option");
    settingMenu_->Draw();
}

void TitleScene::DrawShadow(){}
