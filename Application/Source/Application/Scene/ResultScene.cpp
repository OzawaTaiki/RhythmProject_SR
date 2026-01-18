#include "ResultScene.h"

#include <Application/Scene/Data/SceneDatas.h>
#include <Features/Scene/Manager/SceneManager.h>

#include <Framework/LayerSystem/LayerSystem.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Features/UI/UINavigationManager.h>
#include <Features/Event/EventManager.h>

ResultScene::ResultScene()
{
    EventManager::GetInstance()->AddEventListener("ResultToTitle", this);
    EventManager::GetInstance()->AddEventListener("Retry", this);
}

ResultScene::~ResultScene()
{
    UINavigationManager::GetInstance()->ClearFocus();

    EventManager::GetInstance()->RemoveEventListener("ResultToTitle", this);
    EventManager::GetInstance()->RemoveEventListener("Retry", this);
}

void ResultScene::Initialize(SceneData* sceneData)
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

    // ------------------
    // application

    resultData_.judgeResult[JudgeType::Perfect] = -1;
    resultData_.judgeResult[JudgeType::Good] = -1;
    resultData_.judgeResult[JudgeType::Bad] = -1;
    resultData_.judgeResult[JudgeType::Miss] = -1;


    if (sceneData)
    {
        auto data = dynamic_cast<GameToResultData*>(sceneData);
        if (data)
        {
            resultData_ = data->resultData;
        }
    }

    resultEffectFlow_ = std::make_unique<ResultEffectFlow>();
    resultEffectFlow_->Initialize(resultData_);


    gameEnvironment_ = std::make_unique<GameEnvironment>();
    gameEnvironment_->Initialize();

    boxFilter_ = std::make_unique<BoxFilter>();
    boxFilter_->Initialize();

    boxFilterData_.kernelSize = 11; // カーネルサイズを設定

    boxFilter_->SetData(&boxFilterData_);


    LayerSystem::CreateLayer("GameEnvironment", 0);
    LayerSystem::CreateLayer("Main", 1);
    LayerSystem::CreateOutputLayer("BoxFilter");
}

void ResultScene::Update()
{
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    lightGroup_->ImGui();

#endif // _DEBUG

    float deltaTime = static_cast<float>(GameTime::GetInstance()->GetDeltaTime());

    resultEffectFlow_->Update(deltaTime);
    gameEnvironment_->Update(deltaTime, nullptr, nullptr, 0.0f);

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
}

void ResultScene::Draw()
{
    ModelManager::GetInstance()->PreDrawForObjectModel();
    LayerSystem::SetLayer("GameEnvironment");

    gameEnvironment_->Draw(&SceneCamera_);

    LayerSystem::ApplyPostEffect("GameEnvironment", "BoxFilter", boxFilter_.get());

    LayerSystem::SetLayer("Main");

    ModelManager::GetInstance()->PreDrawForObjectModel();
    resultEffectFlow_->Draw3D();
    resultEffectFlow_->Draw2D();
}

void ResultScene::DrawShadow() {}

void ResultScene::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "ResultToTitle")
    {
        UINavigationManager::GetInstance()->ClearFocus();
        SceneManager::GetInstance()->ReserveScene("TitleScene", nullptr);
    }
    else if (event.GetEventType() == "Retry")
    {
        UINavigationManager::GetInstance()->ClearFocus();
        SceneManager::GetInstance()->ReserveScene("GameScene", nullptr);
    }
}
