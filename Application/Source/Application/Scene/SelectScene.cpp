#include "SelectScene.h"

#include <Features/Event/EventManager.h>
#include <Features/Scene/Manager/SceneManager.h>
#include <Utility/FileDialog/FileDialog.h>
#include <Utility/StringUtils/StringUitls.h>


#include <Application/MusicList/MusicListManager.h>
#include <Application/Scene/Data/SceneDatas.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Application/BGMChangeEventData.h>

using namespace Engine;

SelectScene::SelectScene()
{
    EventManager::GetInstance()->AddEventListener("StartGame", this);
    EventManager::GetInstance()->AddEventListener("BGMChanged", this);
}

SelectScene::~SelectScene()
{
    EventManager::GetInstance()->RemoveEventListener("StartGame", this);
    EventManager::GetInstance()->RemoveEventListener("BGMChanged", this);
}

void SelectScene::Initialize([[maybe_unused]] SceneData* sceneData)
{
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0, 5, -13 };
    SceneCamera_.rotate_ = { 0.26f, 0, 0 };
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

    auto data = dynamic_cast<TitleToSelectData*>(sceneData);
    if (data)
    {
        spectrumRing_ = data->spectrumRing;
        voiceInstance_ = data->voiceInstance;
        lobbyCamera_ = std::move(data->lobbyCamera);
        backImage_ = data->titleBackground;
        backImageAnimation_ = data->titleBackgroundAnimation;
        hexagonGrid_ = std::move(data->hexagonGrid);
    }

    selectUI_ = std::make_unique<SelectUI>();
    selectUI_->Initialize(voiceInstance_);

    LayerSystem::CreateLayer("back", 0);
    LayerSystem::CreateLayer("ring", 40);
    LayerSystem::CreateLayer("ui", 60);

}

void SelectScene::Update()
{
    float deltaTime = Time::GetDeltaTime<float>();

    selectUI_->Update(deltaTime);

    if (spectrumRing_)
    {

        spectrumRing_->Update(selectUI_->GetMusicElapsedTime());
    }

    backImageAnimation_.Update(deltaTime);
    backImage_->Update();
#ifndef _DEBUG
    if (hexagonGrid_)
        hexagonGrid_->Update();
#endif

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
        lobbyCamera_->Update(deltaTime);
        SceneCamera_ = *lobbyCamera_->GetCamera();
        SceneCamera_.UpdateMatrix();
    }

    particleSystem_->Update();
}

void SelectScene::Draw()
{
    LayerSystem::SetLayer("back");
    {
        backImage_->Draw();
        if (hexagonGrid_)
            hexagonGrid_->Draw();
    }
    LayerSystem::SetLayer("ring");
    {
        ModelManager::GetInstance()->PreDrawForObjectModel();
        if (spectrumRing_)
            spectrumRing_->Draw(&SceneCamera_);
    }
    LayerSystem::SetLayer("ui");
    {
        selectUI_->Draw();
    }
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
    else if ("BGMChanged" == eventType)
    {
        auto data = dynamic_cast<BGMChangeEventData*>(event.GetData());
        if (data)
        {
            if (spectrumRing_)
                spectrumRing_->SetMusicInstance(data->newBGM);
        }
    }
}
