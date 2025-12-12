#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Features/TextRenderer/TextGenerator.h>

#include <Features/UI/UIButton.h>

/// <summary>
/// 選曲画面のシーンクラス。
/// </summary>
class SelectScene : public BaseScene
{
public:
    SelectScene() = default;
    ~SelectScene() override = default;


    /// <summary>
    /// シーンの初期化処理。
    /// </summary>
    void Initialize(SceneData* sceneData) override;

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


private:
    // シーン関連
    Camera SceneCamera_ = {};
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleSystem_ = nullptr;

    TextGenerator text_;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;

    //std::unique_ptr<UIButton> selectButton_ = nullptr;

};