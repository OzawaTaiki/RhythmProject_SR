#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>

#include <Features/TextRenderer/TextGenerator.h>
#include <Application/Setting/SettingMenu.h>
#include <Application/SpectrumRing/SpectrumRing.h>
#include <Application/BeatsManager/BeatManager.h>
#include <Features/BPMDetector/BPMDetector.h>

class TitleScene : public BaseScene
{
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void Initialize(SceneData* _sceneData) override;

    void Update() override;

    void Draw() override;
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


    /// ---------------------------------
    ///     application

    std::shared_ptr<SoundInstance> soundInstance_ = nullptr;
    std::shared_ptr<VoiceInstance> voiceInstance_ = nullptr;

    TextGenerator textGenerator_;

    std::unique_ptr<SettingMenu> settingMenu_ = nullptr; // 設定メニュー
    std::unique_ptr<SpectrumRing> spectrumRing_ = nullptr;

    std::unique_ptr<BeatManager> beatManager_ = nullptr;
    std::unique_ptr<BPMDetector> bpmDetector_ = nullptr; // BPM検出器

};