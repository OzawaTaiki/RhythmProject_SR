#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Features/Event/EventListener.h>


#include <Application/Result/ResultData.h>
#include <Application/Result/UI/ResultUI.h>
#include <Application/Result/ResultEffectFlow.h>

#include <Application/GameEnvironment/GameEnvironment.h>

#include <Features/PostEffects/BoxFilter.h>

/// <summary>
/// リザルト画面のシーンクラス。
/// </summary>
class ResultScene : public Engine::BaseScene,
    public Engine::iEventListener
{
public:
    ResultScene();
    ~ResultScene()override;

    /// <summary>
    /// シーンの初期化処理。
    /// </summary>
    void Initialize(Engine::SceneData* sceneData) override;

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理。
    /// </summary>
    void Draw() override;

    /// <summary>
    /// シャドウ描画処理。
    /// </summary>
    void DrawShadow() override;


    void OnEvent(const Engine::GameEvent& event) override;


private:
    // シーン関連
    Engine::Camera SceneCamera_ = {};
    Engine::DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    Engine::LineDrawer* lineDrawer_ = nullptr;
    Engine::Input* input_ = nullptr;
    Engine::ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<Engine::LightGroup> lightGroup_ = nullptr;


    // -----------------------
    // Application

    ResultData resultData_ = {}; // 結果データ

    std::unique_ptr<ResultEffectFlow> resultEffectFlow_ = nullptr; // 結果エフェクトフロー

    std::unique_ptr<GameBackground> gameBackground_ = nullptr; // ゲーム環境

    std::unique_ptr<Engine::BoxFilter> boxFilter_ = nullptr; // ポストエフェクトのボックスフィルター
    Engine::BoxFilterData boxFilterData_ = {}; // ボックスフィルターのデータ
};