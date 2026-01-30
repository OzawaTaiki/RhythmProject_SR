#include "SelectScene.h"

#include <Utility/FileDialog/FileDialog.h>
#include <Utility/StringUtils/StringUitls.h>
#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Event/EventManager.h>

#include <Application/Scene/Data/SceneDatas.h>
#include <Application/MusicList/MusicListManager.h>
#include <Features/Model/Manager/ModelManager.h>

SelectScene::SelectScene()
{
    EventManager::GetInstance()->AddEventListener("StartGame", this);
}

SelectScene::~SelectScene()
{
    EventManager::GetInstance()->RemoveEventListener("StartGame", this);
}

void SelectScene::Initialize([[maybe_unused]] SceneData* sceneData)
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


    ///------------------------------

    MusicListManager::GetInstance()->LoadAync();

    selectUI_ = std::make_unique<SelectUI>();
    selectUI_->Initialize();

    auto data = dynamic_cast<TitleToSelectData*>(sceneData);
    if (data)
    {
        spectrumRing_ = std::move(data->spectrumRing);
        voiceInstance_ = data->voiceInstance;
    }
}

void SelectScene::Update()
{
    float deltaTime = Time::GetDeltaTime<float>();
    selectUI_->Update(deltaTime);

    spectrumRing_->Update(voiceInstance_->GetElapsedTime());
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    if (enableDebugCamera_)
    {
        debugCamera_.Update();
        SceneCamera_.matView_ = debugCamera_.matView_;
        SceneCamera_.TransferData();
    }
    else
#endif // _DEBUG
    {
        SceneCamera_.Update();
        SceneCamera_.UpdateMatrix();
    }

    particleSystem_->Update();

}

void SelectScene::Draw()
{
    selectUI_->Draw();
    ModelManager::GetInstance()->PreDrawForObjectModel();
    spectrumRing_->Draw(&SceneCamera_);
}

void SelectScene::DrawShadow() {}

void SelectScene::OnEvent(const GameEvent& event)
{
    const std::string& eventType = event.GetEventType();
    if (eventType == "StartGame")
    {
        auto data = dynamic_cast<MusicSelectUIEventData*>(event.GetData());
        if (data)
        {
            auto sceneData = std::make_unique<SelectToGameData>();
            sceneData->selectedBeatMapFilePath = data->selectedFilePath;
            SceneManager::ReserveScene("GameScene", std::move(sceneData));
        }
    }
}
