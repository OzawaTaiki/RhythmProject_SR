#include "SampleFramework.h"

#include "SampleScene.h"
#include <Features/Scene/ParticleTestScene.h>
#include "SceneFactory.h"

#include <Essential/ParticleModifierFactory.h>

#include <Framework/LayerSystem/LayerSystem.h>

#include <Features/Model/Primitive/Builder/PrimitiveBuilder.h>
#include <Features/UI/Collider/UICollisionManager.h>
#include <Features/UI/UINavigationManager.h>
#include <Application/Scene/Transition/SceneTransHex.h>

#include <System/Audio/SoundEngine.h>
#include <System/Audio/AudioEffectManager.h>

using namespace Engine;

void SampleFramework::Initialize([[maybe_unused]] const std::wstring& _winTitle)
{
    Framework::Initialize(L"");


    //rtvManager_->CreateRenderTarget("default", WinApp::kWindowWidth_, WinApp::kWindowHeight_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(0.4625f, 0.925f, 0.4625f, 1.0f), false);
    rtvManager_->CreateRenderTarget("default", WinApp::kWindowWidth_, WinApp::kWindowHeight_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(0.0190f, 0.0190f, 0.0933f, 1.0f), false);
    rtvManager_->CreateRenderTarget("ShadowMap", 4096, 4096, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,  Vector4(1.0f, 1.0f, 1.0f, 1.0f),true);

    sceneManager_->SetSceneFactory(std::make_unique<SceneFactory>());

    particleManager_->SetModifierFactory(std::make_unique<ParticleModifierFactory>());

    collisionManager_->Initialize(Vector2(100, 100), 5, Vector2(-50, -50), 1.0f);

    PrimitiveBuilder::BuildAndRegisterAll();

    LayerSystem::Initialize();

    Setting::Load();
    sceneManager_->SetTransition(std::make_unique<SceneTransHex>());

    GenerateModels();

    AudioSystem::GetInstance()->SetMasterVolume(Setting::current_.masterVolume);

    // AudioEffectManager: エフェクト定義を JSON からロード
    AudioEffectManager::GetInstance()->Initialize();
    AudioEffectManager::GetInstance()->LoadEffectData("Resources/Sounds/AudioEffects.json");

    // SoundEngine: サウンド定義を JSON から一括ロード
    SoundEngine::GetInstance()->Initialize();
    SoundEngine::GetInstance()->LoadSoundData("Resources/Sounds/SoundData.json");
    SoundEngine::GetInstance()->LoadEventData("Resources/Sounds/SoundEvents.json");

    FontCache::GetInstance()->GetAtlasData("Resources/Fonts/NotoSansJP-Regular.ttf", 128);


    // 最初のシーンで初期化
    sceneManager_->Initialize("TitleScene");
}

void SampleFramework::Update()
{

    Framework::Update();

    rtvManager_->ClearAllRenderTarget();


    //========== 更新処理 =========

    sceneManager_->Update();
    //particleManager_->Update(); TODO ; 引数のカメラの回転をなんとかしたい
    UINavigationManager::GetInstance()->HandleInput();
    UICollisionManager::GetInstance()->CheckCollision(input_->GetMousePosition());

    //=============================

}

void SampleFramework::Draw()
{

    Framework::PreDraw();

    //rtvManager_->SetDepthStencil("ShadowMap");
    //sceneManager_->DrawShadow();

    rtvManager_->SetRenderTexture("default");

    // ========== 描画処理 ==========
    sceneManager_->Draw();

    ParticleSystem::GetInstance()->DrawParticles();
    batch2DRenderer_->Render();
    lineDrawer_->Draw();

    textRenderer_->EndFrame();
    //=============================

    // レイヤーの合成
    LayerSystem::CompositeAllLayers("default");
    dxCommon_->PreDraw();
    // スワップチェインに戻す
    rtvManager_->SetSwapChainRenderTexture(dxCommon_->GetSwapChain());

    PSOManager::GetInstance()->SetPipeLineStateObject(PSOFlags::Type::OffScreen);
    PSOManager::GetInstance()->SetRootSignature(PSOFlags::Type::OffScreen);

    // レンダーテクスチャを描画
    rtvManager_->DrawRenderTexture("default");

    Framework::PostDraw();

    // 後にupdateに
    sceneManager_->ChangeScene();

}

void SampleFramework::Finalize()
{
    Setting::Save();

    AudioEffectManager::GetInstance()->Finalize();
    SoundEngine::GetInstance()->Finalize();

    Framework::Finalize();
}

void SampleFramework::GenerateModels()
{// 2x2 y+向き
    Plane plane;
    plane.SetSize(Vector2(1.0f, 1.0f) * 2);
    plane.SetNormal(Vector3(0, 1, 0));
    plane.SetPivot(Vector3(0, 0, 0));

    plane.Generate("pY1x1Plane");


    Plane plane_nz1x1;
    plane.SetSize(Vector2(1.0f, 1.0f));
    plane.SetNormal(Vector3(0, 1, 0));
    plane.SetPivot(Vector3(0, 0, 0));

    plane.Generate("pZ1x1Plane");

    Plane plane_py0n1;
    plane_py0n1.SetSize(Vector2(1.0f, 1.0f) * 2);
    plane_py0n1.SetNormal(Vector3(0, 0, -1));
    plane_py0n1.SetPivot(Vector3(0, -1, 0));
    plane_py0n1.SetFlipV(true);

    plane_py0n1.Generate("plane_py0n1");

    Plane plane_py01;
    plane_py01.SetSize(Vector2(1.0f, 1.0f));
    plane_py01.SetNormal(Vector3(0, 1, 0));
    plane_py01.SetPivot(Vector3(0, 1, 0));
    plane_py01.SetFlipV(true);
    plane_py01.Generate("pY1x1p01Plane");// y+向き 1x1 pivot(0,1,0)

    // ほそ長いやつ
    Plane plane2;
    plane2.SetSize(Vector2(0.1f, 0.7f) * 5.0f);
    plane2.SetNormal(Vector3(0, 0, -1));
    plane2.SetPivot(Vector3(0, 0, 0));

    plane2.Generate("nZ0.1x0.7Plane");


    Triangle triangle;
    triangle.SetNormal(Vector3(0, 0, -1));
    triangle.SetVertices({
        Vector3(0, 0.5f, 0),
        Vector3(0.5f, -0.5f, 0),
        Vector3(-0.5f, -0.5f, 0)
                         });
    triangle.Generate("nZ1_1Triangle");

    Ring ring(0.0f, 1.0f);
    ring.SetDivide(64);
    ring.SetUVMode(RingUVMode::Planar);
    ring.SetUVRange(0.3f, 0.3f, 0.7f, 0.7f);
    ring.Generate("i0o1_PlanarRing");
}
