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

#include <Application/Scene/Data/SceneDatas.h>
#include <Application/Setting/Setting.h>

using namespace Engine;
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
    EventManager::GetInstance()->AddEventListener("MusicEnded", this);


}

GameScene::~GameScene()
{
    EventManager::GetInstance()->RemoveEventListener("ValueChanged", this);
    EventManager::GetInstance()->RemoveEventListener("RequestResume", this);
    EventManager::GetInstance()->RemoveEventListener("RequestRetry", this);
    EventManager::GetInstance()->RemoveEventListener("RequestToTitle", this);
    EventManager::GetInstance()->RemoveEventListener("MusicEnded", this);
    //if(loadingThread_.joinable())
    //    loadingThread_.join();

}

// TODO ; やりたいこと にゅうりょく精度アップ
// 別スレッドで入力取得するように。 それと入力された時間を記録

void GameScene::Initialize(SceneData* sceneData)
{
    auto now = std::chrono::system_clock::now();
    Debug::Log(std::format("GameScene Initialize Start TimePoint: {}\n", now.time_since_epoch().count()));
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
    std::string beatMapFilePath = "Resources/Data/Game/BeatMap/Twilight_Memory.json"; // デフォルトの譜面ファイルパス
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

    lightGroup_ = std::make_shared<LightGroup>();
    lightGroup_->Initialize();

    LightingSystem::GetInstance()->SetActiveGroup(lightGroup_);
    auto begin = std::chrono::system_clock::now();
    gameBackground_ = std::make_unique<GameBackground>();
    gameBackground_->Initialize("Resources/Data/Game/haikei.json");
    auto end = std::chrono::system_clock::now();
    Debug::Log(std::format("GameBackground Load Time: {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()));
    Load(beforeScene, beatMapFilePath, editorBeatMapData);

    hasReservedTransition_ = false;
}

void GameScene::Update()
{

    float deltaTime = static_cast<float>(GameTime::GetInstance()->GetDeltaTime());


    gameInputManager_->Update(); // 入力更新

    // ロード完了してなかったら更新しない
    if (!gameMusic_ || !gameMusic_->GetSoundInstance())
        return;

    gameBackground_->Update(deltaTime,
                             audioSpectrum_.get(),
                             gameMusic_->GetSoundInstance().get(),
                             gameMusic_->GetElapsedTime());

    float elapsedTime = gameMusic_->GetElapsedTime();
    audioSpectrum_->GetSpectrumAtTime(elapsedTime);


    switch (currentState_)
    {
        case SceneState::Loading:
            UpdateLoading();
            break;
        case SceneState::WaitingToStart:
            UpdateWaiting(deltaTime);
            break;
        case SceneState::Playing:
            UpdatePlaying(deltaTime);
            break;
        case SceneState::Paused:
            UpdatePaused();
            break;
        case SceneState::GameComplete:
            UpdateCompleted(deltaTime);
            break;
        case SceneState::ResultTransition:
            break;
        default:
            break;
    }

    laneOutline_->Update(gameCore_->GetCombo());

    UpdateCamera();

    particleSystem_->Update();

    if (gameMode_ == GameMode::EditorTest)
    {
        if (input_->IsKeyTriggered(DIK_ESCAPE))
        {
            UINavigationManager::GetInstance()->ClearFocus();
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
    LayerSystem::SetLayer("GameBackground");
    {
        ModelManager::GetInstance()->PreDrawForObjectModel();
        gameBackground_->Draw(&SceneCamera_);
        LayerSystem::ApplyPostEffect("GameBackground", "Bloom", bloom_.get());
        feedbackEffect_->ApplyMissedVignetteEffect("GameBackground", "Vignette");
    }

    if (!isLoadComplete_)
        return;
    ModelManager::GetInstance()->PreDrawForObjectModel();
    LayerSystem::SetLayer("GameCore");
    {
        auto renderer = Text3DRenderer::GetInstance();
        renderer->BeginFrame();
        gameCore_->Draw(&SceneCamera_, gameInputManager_->GetKeyBinds());

        renderer->EndFrame();
        gameUI_->Draw(); // UIの描画
        laneOutline_->Apply("GameCore", "DepthOutline");
    }

    ModelManager::GetInstance()->PreDrawForObjectModel();
    LayerSystem::SetLayer("FeedbackEffect");
    {
        feedbackEffect_->Draw();
    }
    LayerSystem::SetLayer("PauseMenu");
    {
        gameCompleteEffect_->Draw();
        pauseMenu_->Draw();
        settingMenu_->Draw();
    }

    LayerSystem::SetLayer("FeedbackEffect");

}

void GameScene::DrawShadow() {}

void GameScene::UpdateCamera()
{
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

bool GameScene::IsCompleteLoadBeatMap()
{
    if (beatMapLoader_->IsLoading())
    {
        Debug::Log("Loading BeatMap...\n");
    }
    else if (!isBeatMapLoaded_)
    {
        auto begin = std::chrono::system_clock::now();
        Debug::Log(std::format("BeatMap Load Complete TimePoint: {}\n", begin.time_since_epoch().count()));
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

        gameBackground_->SetBPM(beatMapLoader_->GetLoadedBeatMapData().bpm);

        // ロード完了
        Debug::Log("BeatMap Loaded Successfully\n");

        if (gameMusic_)
        {
            gameCore_->SetGameMusic(gameMusic_.get());
            gameInputManager_->SetGameMusic(gameMusic_.get()); // 入力管理に音声インスタンスを設定
            //spectrumTextureGenerator_->MakeLogRanges(gameMusic_->GetSoundInstance()->GetSampleRate(), 60.0f, 6000.0f);
            audioSpectrum_->SetAudioData(gameMusic_->GetSoundInstance()->GetAudioData());
            audioSpectrum_->SetSampleRate(gameMusic_->GetSoundInstance()->GetSampleRate());
        }
        else
        {
            Debug::Log("Error: Failed to create voice instance for sound: " + audioFilePath + "\n");
            assert(false);
        }

        isBeatMapLoaded_ = true;
        isWatingForStart_ = true; // 譜面読み込み完了したら開始待機状態にする

        auto end = std::chrono::system_clock::now();
        Debug::Log(std::format("BeatMap Load Time: {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()));
        Debug::Log(std::format("TimeStamp at Load Complete: {}\n", end.time_since_epoch().count()));

    }
    return isBeatMapLoaded_;
}

void GameScene::UpdateGameStartOffset(float deltaTime)
{
    if (!isWatingForStart_)
        return;

    // 時間になったら 音楽等再生 trueを返す
        // タイマー更新
    waitTimer_ += deltaTime;
    // ゲーム開始オフセット時間を超えたら
    if (waitTimer_ >= gameStartOffset_)
    {
        auto timeStamp = std::chrono::system_clock::now();
        Debug::Log(std::format("Game Start TimePoint: {}\n", timeStamp.time_since_epoch().count()));
        isWatingForStart_ = false;
        waitTimer_ = 0.0f;
        // ゲーム開始
        beatManager_->SetMusicVoiceInstance(gameMusic_->GetVoiceInstance());
        beatManager_->Start();
        gameCore_->Start();

        if (gameMusic_)
        {
            if (gameMusic_->IsPlaying())
                gameMusic_->Pause();
            gameMusic_->Play(Setting::current_.musicVolume);
        }

        currentState_ = SceneState::Playing;
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
    currentState_ = SceneState::WaitingToStart;
    waitTimer_ = 0.0f;
}

void GameScene::ToTitle()
{
    UINavigationManager::GetInstance()->ClearFocus();
    SceneManager::ReserveScene("TitleScene", nullptr);
}

void GameScene::ToResult()
{
    auto data = std::make_unique<GameToResultData>();
    data->resultData.musicTitle = beatMapLoader_->GetLoadedBeatMapData().title; // 譜面のタイトルを取得
    data->resultData.combo = gameCore_->GetMaxCombo();
    data->resultData.score = gameCore_->GetScore();
    data->resultData.judgeResult = gameCore_->GetJudgeResult();
    data->resultData.rank = gameCore_->GetRank();

    UINavigationManager::GetInstance()->ClearFocus();
    SceneManager::ReserveScene("ResultScene", std::move(data)); // 結果シーンにデータを渡す
    hasReservedTransition_ = true;
}

void GameScene::OnEvent(const GameEvent& event)
{
    std::string eventType = event.GetEventType();

    // ポーズメニューからのイベント
    if (StringUtils::Contains(eventType, "Request"))
    {
        if (eventType == "RequestResume")
        {
            gameMusic_->ResumeWithRewind(Setting::current_.musicVolume);
            currentState_ = SceneState::Playing;
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
        {
            if (data->name == "NoteSpeed")
            {
                gameCore_->SetNoteSpeed(data->value);
            }
            else if (data->name == "AudioLatency")
            {
                gameCore_->SetMusicLatency(data->value);
            }
        }
    }

    else if (eventType == "MusicEnded")
    {
        gameMusic_->MusicEnd();
        gameCompleteEffect_->StartEffect(gameCore_->GetJudgeResult());

        currentState_ = SceneState::GameComplete;
    }
}

void GameScene::Load(const std::string& beforeScene, const std::string& filepth, const BeatMapData& data)
{
    auto now = std::chrono::system_clock::now();
    Debug::Log(std::format("GameScene Load Start TimePoint: {}\n", now.time_since_epoch().count()));

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


    ///---------------------------------
    /// Application


    std::string beatMapFilePath = "Resources/Data/Game/BeatMap/Twilight_Memory.json"; // デフォルトの譜面ファイルパス
    //std::string beatMapFilePath = "Resources/Data/Game/BeatMap/demo1.json"; // デフォルトの譜面ファイルパス
    gameMode_ = GameMode::Normal;
    if (beforeScene == "SelectScene")
    {
        beatMapFilePath = filepth; // 選択された譜面ファイルパスを取得
        gameMode_ = GameMode::Normal;
    }
    else if (beforeScene == "EditorScene")
    {
        currentBeatMapData_ = data; // エディタから渡された譜面データを取得
        if (currentBeatMapData_.title == "None")
            currentBeatMapData_.title = "test";
        gameMode_ = GameMode::EditorTest;
    }

    gameCore_ = std::make_unique<GameCore>(); // レーン数はデフォで4
    gameCore_->Initialize(Setting::current_.noteSpeed, Setting::current_.audioLatencyMs, gameStartOffset_); // ノーツの移動速度とオフセット時間を設定
    // TODO : startOffsetの排除
    // stateでの管理に移行中。waitの際にcoreを更新しなければ不要なはず


    gameInputManager_ = std::make_unique<GameInputManager>();
    gameInputManager_->Initialize(input_);

    beatMapLoader_ = BeatMapLoader::GetInstance();

    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100);



    gameUI_ = std::make_unique<GameUI>();
    gameUI_->Initialize();

    pauseMenu_ = std::make_unique<PauseMenu>();
    pauseMenu_->Initialize();

    settingMenu_ = std::make_unique<SettingMenu>();
    settingMenu_->Initialize();

    gameCompleteEffect_ = std::make_unique<GameCompleteEffect>();
    gameCompleteEffect_->Initialize();

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

    LayerSystem::CreateLayer("GameBackground", 0);
    LayerSystem::CreateLayer("GameCore", 10);
    LayerSystem::CreateLayer("FeedbackEffect", 20, PSOFlags::BlendMode::Add);
    LayerSystem::CreateLayer("PauseMenu", 30);
    LayerSystem::CreateOutputLayer("Vignette");
    LayerSystem::CreateOutputLayer("Bloom");
    LayerSystem::CreateOutputLayer("DepthOutline");

    feedbackEffect_ = std::make_unique<FeedbackEffect>();
    feedbackEffect_->Initialize(&SceneCamera_, gameCore_->GetLaneCount(), gameBackground_.get());

    gameCore_->SetJudgeCallback([&](int32_t laneIndex, JudgeType judgeType, int32_t combo)
                                {
                                    feedbackEffect_->PlayJudgeEffect(laneIndex, judgeType, combo);
                                    if (judgeType != JudgeType::Miss)
                                        gameMusic_->DisableBitCrush();
                                });
    gameCore_->SetMissCallback([&]()
                               {
                                   feedbackEffect_->PlayMissedEffect();
                                   int32_t combo = gameCore_->GetCombo();
                                   float duckDepth = feedbackEffect_->GetComboThresholds()->GetMissEffectThreshold(combo);
                                   //float duration = 0.5f;
                                   float duration = beatManager_->GetSecondsPerBeat() * 0.5f;
                                   gameMusic_->TriggerDucking(duckDepth, duration);
                                   gameMusic_->EnableBitCrush();
                               });
    gameCore_->SetHoldCallback([&](int32_t laneIndex) { feedbackEffect_->PlayHoldEffect(laneIndex); });


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

    audioSpectrum_ = std::make_unique<AudioSpectrum>(32768);

    while (!IsCompleteLoadBeatMap())
        std::this_thread::sleep_for(std::chrono::milliseconds(10));


    feedbackEffect_->InitComboThresholds(static_cast<int32_t>(beatMapLoader_->GetLoadedBeatMapData().notes.size()));
    laneOutline_->SetComboThresholds(feedbackEffect_->GetComboThresholds());


    isLoadComplete_ = true;

    auto end = std::chrono::system_clock::now();
    Debug::Log(std::format("GameScene Load Time: {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count()));
}

void GameScene::UpdateLoading()
{
    if (IsCompleteLoadBeatMap())
    {
        currentState_ = SceneState::WaitingToStart;
    }

    // ロード完了してなかったら更新しない
    if (!gameMusic_ || !gameMusic_->GetSoundInstance())
        return;

}

void GameScene::UpdateWaiting(float deltaTime)
{
    UpdateGameStartOffset(deltaTime);

    gameCore_->Update(deltaTime, gameInputManager_->GetInputData());
    feedbackEffect_->Update(deltaTime, gameInputManager_->GetInputData());
    gameUI_->Update(gameCore_->GetCombo(), deltaTime); // コンボ値をUIに渡す
}

void GameScene::UpdatePlaying(float deltaTime)
{
    if (input_->IsKeyPressed(DIK_LCONTROL) && input_->IsKeyTriggered(DIK_R))
    {
        Retry();
    }

    pauseMenu_->Update();
    if (pauseMenu_->IsActive())
    {
        currentState_ = SceneState::Paused;
        return;
    }

    gameMusic_->Update(deltaTime);

    //beatManager_->Update();
    gameCore_->Update(deltaTime, gameInputManager_->GetInputData());

    feedbackEffect_->Update(deltaTime, gameInputManager_->GetInputData());
    gameUI_->Update(gameCore_->GetCombo(), deltaTime); // コンボ値をUIに渡す
}

void GameScene::UpdateCompleted(float deltaTime)
{
    gameCompleteEffect_->Update(deltaTime);

    if (gameCompleteEffect_->IsEffectComplete())
    {
        currentState_ = SceneState::ResultTransition;
        ToResult();
    }
}

void GameScene::UpdatePaused()
{
    gameMusic_->Pause();

    pauseMenu_->Update();
    settingMenu_->Update();
    if (!pauseMenu_->IsActive() && !settingMenu_->IsActive())
    {
        // どちらのメニューも非アクティブな場合の処理（ゲームプレイに戻るなど）
    }
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
    if (ImGuiDebugManager::GetInstance()->Begin("GameScene"))
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
            gameBackground_->SetBPM(bpm);
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
