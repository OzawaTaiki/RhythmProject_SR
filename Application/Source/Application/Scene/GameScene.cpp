#include "GameScene.h"
#include <Features/Model/Manager/ModelManager.h>

#include <Features/Model/Primitive/Plane.h>
#include <Features/Model/Primitive/Triangle.h>
#include <Features/Model/Primitive/Ring.h>

#include <Features/Scene/Manager/SceneManager.h>

#include <System/Time/GameTime.h>

#include <Debug/Debug.h>
#include <Debug/ImguITools.h>
#include <Debug/ImGuiDebugManager.h>
#include <Utility/StringUtils/StringUitls.h>
#include <Features/TextRenderer/TextRenderer.h>
#include <Features/Event/EventManager.h>

#include <Application/Scene/Transition/SceneTrans.h>
#include <Application/Scene/Data/SceneDatas.h>
#include <Application/Setting/Setting.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <Features/ColorMask/ColorMask.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Features/WaveformDisplay/WaveformAnalyzer.h>
#include <Features/UI/Collider/UICollisionManager.h>
#include <Features/TextRenderer/Text3DRenderer.h>
#include <Features/UI/UINavigationManager.h>


GameScene::GameScene()
{
    EventManager::GetInstance()->AddEventListener("ValueChanged", this);
    EventManager::GetInstance()->AddEventListener("RequestResume", this);
    EventManager::GetInstance()->AddEventListener("RequestRetry", this);
    EventManager::GetInstance()->AddEventListener("RequestToTitle", this);


}

GameScene::~GameScene()
{
    EventManager::GetInstance()->RemoveEventListener("ValueChanged", this);
    EventManager::GetInstance()->RemoveEventListener("RequestResume", this);
    EventManager::GetInstance()->RemoveEventListener("RequestRetry", this);
    EventManager::GetInstance()->RemoveEventListener("RequestToTitle", this);

}

// TODO ; やりたいこと にゅうりょく精度アップ
// 別スレッドで入力取得するように。 それと入力された時間を記録

void GameScene::Initialize(SceneData* sceneData)
{
    auto now = std::chrono::system_clock::now();

    Debug::Log(std::format("Current TimePoint: {}\n", now.time_since_epoch().count()));

    //SceneCamera_.Initialize();
    //SceneCamera_.translate_ = { 0,5,-13 };
    //SceneCamera_.rotate_ = { 0.26f,0,0 };
    //SceneCamera_.UpdateMatrix();
    //debugCamera_.Initialize();

    //camera2d_.Initialize(CameraType::Orthographic);



    //lineDrawer_ = LineDrawer::GetInstance();
    //lineDrawer_->Initialize();
    //lineDrawer_->SetCameraPtr(&SceneCamera_);
    //lineDrawer_->SetCameraPtr2D(&camera2d_);

    //input_ = Input::GetInstance();

    //particleSystem_ = ParticleSystem::GetInstance();
    //particleSystem_->SetCamera(&SceneCamera_);

    //lightGroup_ = std::make_shared<LightGroup>();
    //lightGroup_->Initialize();

    //LightingSystem::GetInstance()->SetActiveGroup(lightGroup_);

    ///---------------------------------
    /// Application

    std::string beforeScene = "None";
    std::string beatMapFilePath = "Resources/Data/Game/BeatMap/demo1.json"; // デフォルトの譜面ファイルパス
    BeatMapData editorBeatMapData = {};

    gameMode_ = GameMode::Normal;
    if (sceneData)
    {
        beforeScene = sceneData->beforeScene;
        if (beforeScene == "SelectScene")
        {
            auto selectToGameData = dynamic_cast<SelectToGameData*>(sceneData);
            if (selectToGameData)
            {
                beatMapFilePath = selectToGameData->selectedBeatMapFilePath; // 選択された譜面ファイルパスを取得
                gameMode_ = GameMode::Normal;
            }
        }
        else if (beforeScene == "EditorScene")
        {
            auto editorToGameData = dynamic_cast<SharedBeatMapData*>(sceneData);
            if (editorToGameData)
            {
                editorBeatMapData = editorToGameData->beatMapData; // エディタから渡された譜面データを取得
                if (editorBeatMapData.title == "None")                    editorBeatMapData.title = "test";
                gameMode_ = GameMode::EditorTest;
            }
        }
    }
    isLoadComplete_ = false;
    //loadingThread_ = std::thread(&GameScene::Load, this, beforeScene, beatMapFilePath, editorBeatMapData);
    gameEnvironment_ = std::make_unique<GameEnvironment>();
    gameEnvironment_->Initialize();
    Load(beforeScene, beatMapFilePath, editorBeatMapData);
//    gameCore_ = std::make_unique<GameCore>(); // レーン数はデフォで4
//    gameCore_->Initialize(Setting::current_.noteSpeed, Setting::current_.audioLatencyMs); // ノーツの移動速度とオフセット時間を設定
//
//    gameInputManager_ = std::make_unique<GameInputManager>();
//    gameInputManager_->Initialize(input_);
//
//    beatMapLoader_ = BeatMapLoader::GetInstance();
//
//    beatManager_ = std::make_unique<BeatManager>();
//    beatManager_->Initialize(100);
//
//    gameEnvironment_ = std::make_unique<GameEnvironment>();
//    gameEnvironment_->Initialize();
//
//    feedbackEffect_ = std::make_unique<FeedbackEffect>();
//    feedbackEffect_->Initialize(&SceneCamera_, gameCore_->GetLaneCount(), gameEnvironment_.get());
//
//    gameUI_ = std::make_unique<GameUI>();
//    gameUI_->Initialize();
//
//    pauseMenu_ = std::make_unique<PauseMenu>();
//    pauseMenu_->Initialize();
//
//    settingMenu_ = std::make_unique<SettingMenu>();
//    settingMenu_->Initialize();
//
//    gameCore_->SetJudgeCallback([&](int32_t laneIndex, JudgeType judgeType) {feedbackEffect_->PlayJudgeEffect(laneIndex, judgeType); });
//    gameCore_->SetMissCallback([&]() {feedbackEffect_->PlayMissedEffect(); });
//    gameCore_->SetHoldCallback([&](int32_t laneIndex) {feedbackEffect_->PlayHoldEffect(laneIndex); });
//
//    SceneManager::GetInstance()->SetTransition(std::make_unique<SceneTrans>());
//
//    switch (gameMode_)
//    {
//    case GameMode::Normal:
//        beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap(beatMapFilePath);
//        break;
//    case GameMode::EditorTest:
//        beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap(currentBeatMapData_);
//        break;
//    default:
//        break;
//    }
//
//#ifdef _DEBUG
//    beatManager_->SetEnableSound(true); // デバッグ時は音を有効にする
//#else
//    beatManager_->SetEnableSound(false); // デバッグ時以外は音を無効にする
//#endif // _DEBUG
//
//    isBeatMapLoaded_ = false;
//
//    isTransitionToResultScene_ = true;
//
//    isMusicPlaying_ = true;
//
//    LayerSystem::CreateLayer("GameEnvironment",0);
//    LayerSystem::CreateLayer("GameCore", 10);
//    LayerSystem::CreateLayer("FeedbackEffect", 20, PSOFlags::BlendMode::Add);
//    LayerSystem::CreateLayer("PauseMenu", 30);
//    LayerSystem::CreateOutputLayer("Vignette");
//    LayerSystem::CreateOutputLayer("Bloom");
//    LayerSystem::CreateOutputLayer("DepthOutline");
//
//
//    depthBasedOutLine_ = std::make_unique<DepthBasedOutLine>();
//    depthBasedOutLine_->Initialize();
//
//    depthBasedOutLineData_ = DepthBasedOutLineData();
//    depthBasedOutLineData_.edgeColor.z = 0.8f;
//    depthBasedOutLineData_.edgeWidth = 1.5f;
//    depthBasedOutLine_->SetCamera(&SceneCamera_);
//    depthBasedOutLine_->SetData(&depthBasedOutLineData_);
//
//    bloom_ = std::make_unique<Bloom>();
//    bloom_->Initialize();
//    bloomData_ = BloomConstantBufferData();
//    bloomData_.threshold = 0.1f;
//    bloomData_.intensity = 2.0f;
//    bloomData_.softKnee = 0.5f;
//    bloom_->UpdateData(bloomData_);
//    bloomBlurData_ = BloomBlurConstantBufferData();
//    bloomBlurData_.texelSize = Vector2(1.0f / WinApp::kWindowSize_.x, 1.0f / WinApp::kWindowSize_.y);
//    bloomBlurData_.blurRadius = 5.0f;
//    bloom_->UpdateData(bloomBlurData_);
//
//
//    spectrumTextureGenerator_ = std::make_unique<SpectrumTextureGenerator>();
//    spectrumTextureGenerator_->Initialize(Vector4(0, 0, 0, 1));
//
//    audioSpectrum_ = AudioSpectrum();

}

void GameScene::Update()
{
    static auto begin = std::chrono::system_clock::now();

    // ロードが完了してなかったら更新しない
    /*if (!IsCompleteLoadBeatMap())
        return;*/
    float deltaTime = static_cast<float>(GameTime::GetInstance()->GetDeltaTime());
    if (deltaTime > 1.0f)
        deltaTime = 0.1f;
    gameEnvironment_->Update(deltaTime);

    if (!isLoadComplete_)
    {
        Debug::Log("Waiting for BeatMap Load...\n");
        return;
    }

    UpdateGameStartOffset();

#ifdef _DEBUG
    ImGui();

    if(input_->IsKeyTriggered(DIK_F8))
        isTransitionToResultScene_ = !isTransitionToResultScene_; // F8キーで結果シーンへの遷移を切り替え
    if (input_->IsKeyTriggered(DIK_F7))
    {
        noteUpdateEnabled_ = !noteUpdateEnabled_;
        if(noteUpdateEnabled_)
            gameMusic_->Resume(); // ノート更新が有効なら音楽を再生
        else
            gameMusic_->Pause(); // ノート更新が無効なら音楽を一時停止
    }

#endif // _DEBUG

#pragma region Application

    if (input_->IsKeyPressed(DIK_LCONTROL) && input_->IsKeyTriggered(DIK_R))
    {
        Retry();
    }

    //depthBasedOutLineData_.edgeColor.x = std::sin(static_cast<float>((Time::GetCurrentTime)())) * 0.5f + 0.5f;
    laneOutline_->Update(gameCore_->GetCombo());
    pauseMenu_->Update();

    gameInputManager_->Update(); // 入力更新
    if (noteUpdateEnabled_ && !pauseMenu_->IsActive())
    {
        beatManager_->Update();
        gameCore_->Update(deltaTime, gameInputManager_->GetInputData());
    }
    else
    {
        gameMusic_->Pause();
    }
    feedbackEffect_->Update(deltaTime, gameInputManager_->GetInputData());
    gameUI_->Update(gameCore_->GetCombo()); // コンボ値をUIに渡す
    settingMenu_->Update();

    float elapsedTime = gameMusic_->GetElapsedTime();
    float scale = WaveformAnalyzer::GetRMSAtTime(gameMusic_->GetSoundInstance().get(), elapsedTime);
    spectrumTextureGenerator_->Generate(audioSpectrum_.GetSpectrumAtTime(elapsedTime), scale, 48);
    spectrumTextureGenerator_->ReserveClear();

#pragma endregion // Application

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

    if (IsMusicEnd())
    {
        if (isTransitionToResultScene_)
        {
            auto data = std::make_unique<GameToResultData>();
            data->resultData.musicTitle = beatMapLoader_->GetLoadedBeatMapData().title; // 譜面のタイトルを取得
            data->resultData.combo = gameCore_->GetMaxCombo();
            data->resultData.score = gameCore_->GetMaxCombo() * 100; // 仮のスコア計算
            data->resultData.judgeResult = gameCore_->GetJudgeResult();

            UINavigationManager::GetInstance()->ClearFocus();
            SceneManager::ReserveScene("ResultScene", std::move(data)); // 結果シーンにデータを渡す
        }
    }

    if (gameMode_ == GameMode::EditorTest)
    {
        if (input_->IsKeyTriggered(DIK_ESCAPE))
        {
            auto data = std::make_unique<SharedBeatMapData>();
            data->beatMapData = currentBeatMapData_;
            UINavigationManager::GetInstance()->ClearFocus();
            SceneManager::ReserveScene("EditorScene", std::move(data)); // エディタシーンに譜面データを渡す
        }
    }
}

void GameScene::Draw()
{
    // レイヤーごとに描画
    ModelManager::GetInstance()->PreDrawForObjectModel();

    LayerSystem::SetLayer("GameEnvironment");
    {
        ModelManager::GetInstance()->PreDrawForObjectModel();
        gameEnvironment_->Draw(&SceneCamera_);
        LayerSystem::ApplyPostEffect("GameEnvironment", "Bloom",bloom_.get());
        feedbackEffect_->ApplyMissedVignetteEffect("GameEnvironment", "Vignette");
    }

    if (!isLoadComplete_)
        return;

    ModelManager::GetInstance()->PreDrawForObjectModel();
    LayerSystem::SetLayer("GameCore");
    {
        auto renderer = Text3DRenderer::GetInstance();
        renderer->BeginFrame();
        gameCore_->Draw(&SceneCamera_, gameInputManager_->GetKeyBinds());

        Sprite::PreDraw();

        gameUI_->Draw(); // UIの描画
        renderer->EndFrame();
        laneOutline_->Apply("GameCore", "DepthOutline");
    }


    ModelManager::GetInstance()->PreDrawForObjectModel();
    LayerSystem::SetLayer("FeedbackEffect");
    {
        feedbackEffect_->Draw();
    }
    LayerSystem::SetLayer("PauseMenu");
    {
        pauseMenu_->Draw();
        settingMenu_->Draw();
    }

    LayerSystem::SetLayer("FeedbackEffect");

}

void GameScene::DrawShadow() {}

bool GameScene::IsCompleteLoadBeatMap()
{
    if (beatMapLoader_->IsLoading())
    {
        Debug::Log("Loading BeatMap...\n");
    }
    else if (!isBeatMapLoaded_)
    {
        if (!beatMapLoader_->IsLoadingSuccess())
        {
            // 読み込み失敗してたら エラーメッセージを取得して表示
            std::string errorMessage = beatMapLoader_->GetErrorMessage();
            Debug::Log("Error: " + errorMessage + "\n");
            assert(false);
        }

        // 譜面データを渡してnoteを生成
        gameCore_->GenerateNotes(beatMapLoader_->GetLoadedBeatMapData());

        // bpmを設定
        beatManager_->SetBPM(beatMapLoader_->GetLoadedBeatMapData().bpm);
        beatManager_->SetOffset(beatMapLoader_->GetLoadedBeatMapData().offset);
        std::string audioFilePath = beatMapLoader_->GetLoadedBeatMapData().audioFilePath;

        gameMusic_ = std::make_unique<GameMusic>(audioFilePath); // 音楽の管理を初期化
        gameMusic_->Initialize();

        gameEnvironment_->SetBPM(beatMapLoader_->GetLoadedBeatMapData().bpm);

        // ロード完了
        Debug::Log("BeatMap Loaded Successfully\n");

        if (gameMusic_)
        {
            gameCore_->SetGameMusic(gameMusic_.get());
            gameInputManager_->SetGameMusic(gameMusic_.get()); // 入力管理に音声インスタンスを設定
            spectrumTextureGenerator_->MakeLogRanges(gameMusic_->GetSoundInstance()->GetSampleRate(), 60.0f, 6000.0f);
            gameEnvironment_->SetSpectrumTextureHandle(spectrumTextureGenerator_->GetTextureHandle());
            audioSpectrum_.SetAudioData(gameMusic_->GetSoundInstance()->GetAudioData());
            audioSpectrum_.SetSampleRate(gameMusic_->GetSoundInstance()->GetSampleRate());
        }
        else
        {
            Debug::Log("Error: Failed to create voice instance for sound: " + audioFilePath + "\n");
            assert(false);
        }


        isBeatMapLoaded_ = true;
        isWatingForStart_ = true; // 譜面読み込み完了したら開始待機状態にする
    }
    return isBeatMapLoaded_;
}

void GameScene::UpdateGameStartOffset()
{
    if (!isWatingForStart_)
        return;

    // 時間になったら 音楽等再生 trueを返す
        // タイマー更新
    float deltaTime = static_cast<float>(GameTime::GetInstance()->GetDeltaTime());
    waitTimer_ += deltaTime > 1.0f ? 0.1f : deltaTime;
    // ゲーム開始オフセット時間を超えたら
    if (waitTimer_ >= gameStartOffset_)
    {
        isWatingForStart_ = false;
        waitTimer_ = 0.0f;
        // ゲーム開始
        beatManager_->SetMusicVoiceInstance(gameMusic_->GetVoiceInstance());
        beatManager_->Start();
        gameCore_->Start();

        if (gameMusic_)
            gameMusic_->Play(Setting::current_.musicVolume);
    }
}

bool GameScene::IsMusicEnd() const
{
    if (gameMusic_ && gameMusic_->IsMusicEnd())
        return true;

    return false;
}

void GameScene::Retry()
{
    beatManager_->Reset();
    gameCore_->Restart();
    gameMusic_->Pause();
    isWatingForStart_ = true;
}

void GameScene::ToTitle()
{
    UINavigationManager::GetInstance()->ClearFocus();
    SceneManager::ReserveScene("TitleScene", nullptr);
}

void GameScene::OnEvent(const GameEvent& event)
{
    std::string eventType = event.GetEventType();

    // ポーズメニューからのイベント
    if(StringUtils::Contains(eventType, "Request"))
    {
        if (eventType == "RequestResume")
        {
            gameMusic_->ResumeWithRewind(Setting::current_.musicVolume);
        }
        else if (eventType == "RequestRetry")
        {
            Retry();
        }
        else if (eventType == "RequestToTitle")
        {
            ToTitle();
        }
    }

    // 値設定イベント
    else  if (eventType == "ValueChanged")
    {
        auto data = dynamic_cast<ValueChangedEventData*>(event.GetData());
        if (data)
        {if (data->name == "NoteSpeed")
            {
                gameCore_->SetNoteSpeed(data->value);
            }
            else if (data->name == "AudioLatency")
            {
                gameCore_->SetMusicLatency(data->value);
            }
        }
    }
}

void GameScene::Load(const std::string& beforeScene, const std::string& filepth, const BeatMapData& data)
{

    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,5,-13 };
    SceneCamera_.rotate_ = { 0.26f,0,0 };
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

    ///---------------------------------
    /// Application


    std::string beatMapFilePath = "Resources/Data/Game/BeatMap/demo1.json"; // デフォルトの譜面ファイルパス
    gameMode_ = GameMode::Normal;
    if (beforeScene == "SelectScene")
    {
        beatMapFilePath = filepth; // 選択された譜面ファイルパスを取得
        gameMode_ = GameMode::Normal;
    }
    else if (beforeScene == "EditorScene")
    {
        currentBeatMapData_ = data; // エディタから渡された譜面データを取得
        if (currentBeatMapData_.title == "None")                    currentBeatMapData_.title = "test";
        gameMode_ = GameMode::EditorTest;
    }

    gameCore_ = std::make_unique<GameCore>(); // レーン数はデフォで4
    gameCore_->Initialize(Setting::current_.noteSpeed, Setting::current_.audioLatencyMs, gameStartOffset_); // ノーツの移動速度とオフセット時間を設定

    gameInputManager_ = std::make_unique<GameInputManager>();
    gameInputManager_->Initialize(input_);

    beatMapLoader_ = BeatMapLoader::GetInstance();

    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100);


    feedbackEffect_ = std::make_unique<FeedbackEffect>();
    feedbackEffect_->Initialize(&SceneCamera_, gameCore_->GetLaneCount(), gameEnvironment_.get());

    gameUI_ = std::make_unique<GameUI>();
    gameUI_->Initialize();

    pauseMenu_ = std::make_unique<PauseMenu>();
    pauseMenu_->Initialize();

    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    gameCore_->SetJudgeCallback([&](int32_t laneIndex, JudgeType judgeType,int32_t combo) { feedbackEffect_->PlayJudgeEffect(laneIndex, judgeType, combo); });
    gameCore_->SetMissCallback([&]() { feedbackEffect_->PlayMissedEffect(); });
    gameCore_->SetHoldCallback([&](int32_t laneIndex) { feedbackEffect_->PlayHoldEffect(laneIndex); });

    SceneManager::GetInstance()->SetTransition(std::make_unique<SceneTrans>());

    switch (gameMode_)
    {
        case GameMode::Normal:
            beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap(beatMapFilePath);
            break;
        case GameMode::EditorTest:
            beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap(currentBeatMapData_);
            break;
        default:
            break;
    }

#ifdef _DEBUG
    beatManager_->SetEnableSound(true); // デバッグ時は音を有効にする
#else
    beatManager_->SetEnableSound(false); // デバッグ時以外は音を無効にする
#endif // _DEBUG

    isBeatMapLoaded_ = false;

    isTransitionToResultScene_ = true;

    isMusicPlaying_ = true;

    LayerSystem::CreateLayer("GameEnvironment", 0);
    LayerSystem::CreateLayer("GameCore", 10);
    LayerSystem::CreateLayer("FeedbackEffect", 20, PSOFlags::BlendMode::Add);
    LayerSystem::CreateLayer("PauseMenu", 30);
    LayerSystem::CreateOutputLayer("Vignette");
    LayerSystem::CreateOutputLayer("Bloom");
    LayerSystem::CreateOutputLayer("DepthOutline");



    bloom_ = std::make_unique<Bloom>();
    bloom_->Initialize();
    bloomData_ = BloomConstantBufferData();
    bloomData_.threshold = 0.1f;
    bloomData_.intensity = 2.0f;
    bloomData_.softKnee = 0.5f;
    bloom_->UpdateData(bloomData_);
    bloomBlurData_ = BloomBlurConstantBufferData();
    bloomBlurData_.texelSize = Vector2(1.0f / WinApp::kWindowSize_.x, 1.0f / WinApp::kWindowSize_.y);
    bloomBlurData_.blurRadius = 5.0f;
    bloom_->UpdateData(bloomBlurData_);

    laneOutline_ = std::make_unique<LaneOutline>();
    laneOutline_->Initialize(&SceneCamera_);


    spectrumTextureGenerator_ = std::make_unique<SpectrumTextureGenerator>();
    spectrumTextureGenerator_->Initialize(Vector4(0, 0, 0, 1));

    audioSpectrum_ = AudioSpectrum();

    while (!IsCompleteLoadBeatMap())
        continue;

    feedbackEffect_->InitComboThresholds(static_cast<int32_t>(beatMapLoader_->GetLoadedBeatMapData().notes.size()));
    laneOutline_->SetComboThresholds(feedbackEffect_->GetComboThresholds());


    isLoadComplete_ = true;
}

void GameScene::ImGui()
{
#ifdef _DEBUG

    //if (input_->IsKeyTriggered(DIK_SPACE))
    //    gameEnvironment_->StartAnimation();


    if (input_->IsKeyTriggered(DIK_F1))
    {
        enableDebugCamera_ = !enableDebugCamera_;
    }
    if (ImGuiDebugManager::GetInstance()->Begin("Bloom"))
    {
        ImGui::DragFloat("Threshold", &bloomData_.threshold, 0.01f);
        ImGui::DragFloat("Intensity", &bloomData_.intensity, 0.01f);
        ImGui::DragFloat("BlurSigma", &bloomData_.softKnee, 0.01f);
        bloom_->UpdateData(bloomData_);
        ImGui::Separator();
        ImGui::DragFloat("BlurRadius", &bloomBlurData_.blurRadius, 0.01f);
        ImGui::End();
    }
    if(ImGuiDebugManager::GetInstance()->Begin("GameScene"))
    {
        ImGui::Text("combo: %d", gameCore_->GetCombo());
        ImGui::Text("maxCombo: %d", gameCore_->GetMaxCombo());

        float time = gameMusic_->GetElapsedTime();
        ImGui::Text("Elapsed Time: %.2f", time);


        if (ImGui::Button("stop"))
            beatManager_->Stop();
        if (ImGui::Button("play##beat"))
            beatManager_->Start();


        if (ImGui::Button("play##music"))
        {
            gameMusic_->Play(0.3f);
        }
        static float bpm = 120;
        ImGui::DragFloat("BPM", &bpm, 0.1f);
        if (ImGui::Button("SetBPM"))
        {
            beatManager_->SetBPM(bpm);
            gameEnvironment_->SetBPM(bpm);
        }

        static float volume = 0.2f;
        if (ImGui::DragFloat("music Vol", &volume, 0.01f))
            gameMusic_->SetVolume(volume);

        static float offset = 0.6f;
        ImGui::DragFloat("offset", &offset, 0.001f);

        if (input_->IsKeyTriggered(DIK_R))
        {
            gameMusic_->Pause();
            gameMusic_->Play(0.3f);
            gameCore_->Restart(); // ゲームコアに音声インスタンスを設定
            beatManager_->Reset();
        }


        ImGui::Separator();
        ImGui::Text("Music Duration : %.2fsec", gameMusic_->GetDuration());
        if (gameMusic_)
            ImGui::Text("Music elapse   : %.2fsec", gameMusic_->GetElapsedTime());

        static float noteSpeed = 30.0f;
        if (ImGui::DragFloat("Note Speed", &noteSpeed, 0.1f, 0.1f, 100.0f))
        {
            gameCore_->SetNoteSpeed(noteSpeed);
        }

        //if (IsMusicEnd())
            //voiceInstance_ = soundInstance_->Play(volume, 130.1f); // スペースキーで音楽を再生

        feedbackEffect_->DebugWindoow();

        ImGui::End();
    }

    SceneCamera_.ImGui();



#endif // _DEBUG

}
