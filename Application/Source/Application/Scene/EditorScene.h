#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>

/// <summary>
/// エディタ用シーン。
/// </summary>
class EditorScene : public Engine::BaseScene
{
public:
    EditorScene() = default;
    ~EditorScene() override = default;

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
    /// シャドウ用の描画処理。
    /// </summary>
    void DrawShadow() override;


private:
    // シーン関連
    Engine::Camera SceneCamera_ = {};
    Engine::DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    Engine::LineDrawer* lineDrawer_ = nullptr;
    Engine::Input* input_ = nullptr;
    Engine::ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<Engine::LightGroup> lightGroup_ = nullptr;

    // Application
    std::unique_ptr<BeatMapEditor> beatMapEditor_ = nullptr; // ビートマップエディターのインスタンス


};