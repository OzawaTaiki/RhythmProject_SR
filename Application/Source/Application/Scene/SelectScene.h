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
#include <Application/TItileCamera/TitleCamera.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UVTransform/UVTransformAnimation.h>

/// <summary>
/// 選曲画面のシーンクラス。
/// </summary>
class SelectScene : public BaseScene, public iEventListener
{
public:
    SelectScene();
    ~SelectScene() override;

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

    /// <summary>
    /// イベント受信
    /// </summary>
    void OnEvent(const GameEvent& event) override;

private:
    // シーン関連
    Camera SceneCamera_ = {};
    DebugCamera debugCamera_ = {};

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;

#ifdef _DEBUG
    bool enableDebugCamera_ = false;
#endif // _DEBUG

    ///------------------------------

    std::shared_ptr<LobbyCamera> lobbyCamera_ = nullptr;

    std::unique_ptr<SelectUI> selectUI_ = nullptr;

    std::shared_ptr<SpectrumRing> spectrumRing_ = nullptr;
    std::shared_ptr<VoiceInstance> voiceInstance_ = nullptr;

    std::shared_ptr<UIImageElement> backImage_ = nullptr;
    UVTransformAnimation backImageAnimation_ = {};
};