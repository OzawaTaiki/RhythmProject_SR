#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Features/Event/EventListener.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <Features/Scene/Interface/BaseScene.h>
#include <Features/TextRenderer/TextGenerator.h>
#include <Features/UVTransform/UVTransformAnimation.h>
#include <System/Input/Input.h>
#include <System/Time/Stopwatch.h>


#include <Application/BeatsManager/BeatManager.h>
#include <Application/HexagonGrid/HexagonGrid.h>
#include <Application/Setting/SettingMenu.h>
#include <Application/SpectrumRing/SpectrumRing.h>
#include <Application/TitleUI.h>
#include <Application/TitleCamera/TitleCamera.h>

// #include <Features/BPMDetector/BPMDetector.h>

/// <summary>
/// タイトル画面のシーンクラス。
/// </summary>
class TitleScene : public Engine::BaseScene, public Engine::iEventListener
{
public:
    TitleScene();
    ~TitleScene() override;

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
    Engine::Camera camera2d_ = {};
    Engine::DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    Engine::LineDrawer* lineDrawer_ = nullptr;
    Engine::Input* input_ = nullptr;
    Engine::ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<Engine::LightGroup> lightGroup_ = nullptr;

    /// ---------------------------------
    ///     application

    std::unique_ptr<LobbyCamera> lobbyCamera_;

    std::shared_ptr<Engine::SoundInstance> soundInstance_ = nullptr;
    std::shared_ptr<Engine::VoiceInstance> voiceInstance_ = nullptr;

    Engine::TextGenerator textGenerator_;

    std::unique_ptr<SettingMenu> settingMenu_ = nullptr; // 設定メニュー
    std::shared_ptr<SpectrumRing> spectrumRing_ = nullptr;

    std::unique_ptr<BeatManager> beatManager_ = nullptr;

    std::unique_ptr<TitleUI> titleUI_;

    std::shared_ptr<HexagonGrid> hexagonGrid_;

    std::shared_ptr<Engine::UIImageElement> titleBack_;
    Engine::UVTransformAnimation uvAnimation_;
};