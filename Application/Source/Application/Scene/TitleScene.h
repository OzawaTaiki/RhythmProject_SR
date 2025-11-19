#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Features/TextRenderer/TextGenerator.h>
#include <Features/Event/EventListener.h>

#include <Application/Setting/SettingMenu.h>
#include <Application/SpectrumRing/SpectrumRing.h>
#include <Application/BeatsManager/BeatManager.h>
#include <Application/TitleUI.h>
#include <Application/TItileCamera/TitleCamera.h>
#include <Features/UI/UITextBox.h>
//#include <Features/BPMDetector/BPMDetector.h>

/// <summary>
/// タイトル画面のシーンクラス。
/// </summary>
class TitleScene : public BaseScene, public iEventListener
{
public:
    TitleScene();
    ~TitleScene() override;

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


    void OnEvent(const GameEvent& event) override;
private:
    // シーン関連
    Camera SceneCamera_ = {};
    Camera camera2d_ = {};
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;


    /// ---------------------------------
    ///     application

    TitleCamera titleCamera_;

    std::shared_ptr<SoundInstance> soundInstance_ = nullptr;
    std::shared_ptr<VoiceInstance> voiceInstance_ = nullptr;

    TextGenerator textGenerator_;

    std::unique_ptr<SettingMenu> settingMenu_ = nullptr; // 設定メニュー
    std::unique_ptr<SpectrumRing> spectrumRing_ = nullptr;

    std::unique_ptr<BeatManager> beatManager_ = nullptr;

    std::unique_ptr<UITextBox> test_textBox;

    std::unique_ptr<TitleUI> titleUI_;



};