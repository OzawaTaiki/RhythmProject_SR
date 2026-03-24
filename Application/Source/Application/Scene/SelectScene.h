#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Features/Event/EventListener.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <Features/Scene/Interface/BaseScene.h>
#include <Features/TextRenderer/TextGenerator.h>
#include <System/Input/Input.h>
#include <System/Time/Stopwatch.h>


#include <Application/Select/SelectUI.h>
#include <Application/SpectrumRing/SpectrumRing.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UVTransform/UVTransformAnimation.h>
#include <Application/TitleCamera/TitleCamera.h>
#include <Application/HexagonGrid/HexagonGrid.h>

/// <summary>
/// 選曲画面のシーンクラス。
/// </summary>
class SelectScene : public Engine::BaseScene, public Engine::iEventListener
{
public:
    SelectScene();
    ~SelectScene() override;

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

    /// <summary>
    /// イベント受信
    /// </summary>
    void OnEvent(const Engine::GameEvent& event) override;

private:
    // シーン関連
    Engine::Camera SceneCamera_ = {};
    Engine::DebugCamera debugCamera_ = {};

    Engine::LineDrawer* lineDrawer_ = nullptr;
    Engine::Input* input_ = nullptr;
    Engine::ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<Engine::LightGroup> lightGroup_ = nullptr;

#ifdef _DEBUG
    bool enableDebugCamera_ = false;
#endif // _DEBUG

    ///------------------------------

    std::unique_ptr<LobbyCamera> lobbyCamera_ = nullptr;

    std::unique_ptr<SelectUI> selectUI_ = nullptr;

    std::shared_ptr<SpectrumRing> spectrumRing_ = nullptr;
    std::shared_ptr<Engine::VoiceInstance> voiceInstance_ = nullptr;

    std::shared_ptr<Engine::UIImageElement> backImage_ = nullptr;
    Engine::UVTransformAnimation backImageAnimation_ = {};

    std::shared_ptr<HexagonGrid> hexagonGrid_ = nullptr;
};