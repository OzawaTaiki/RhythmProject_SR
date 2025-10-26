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
class EditorScene : public BaseScene
{
public:
    EditorScene() = default;
    ~EditorScene() override = default;

    /// <summary>
    /// シーンの初期化処理。
    /// </summary>
    void Initialize(SceneData* _sceneData) override;

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
    Camera SceneCamera_ = {};
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;

    // Application
    std::unique_ptr<BeatMapEditor> beatMapEditor_ = nullptr; // ビートマップエディターのインスタンス


};