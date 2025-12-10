#pragma once

// 必須
#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>

#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Light/Group/LightGroup.h>

#include <memory>

#include <Features/Sprite/Sprite.h>
#include <Features/Model/ObjectModel.h>
#include <Features/Effect/Emitter/ParticleEmitter.h>

#include <System/Audio/AudioSystem.h>
#include <System/Audio/SoundInstance.h>
#include <system/Audio/VoiceInstance.h>
#include <Features/AudioSpectrum/AudioSpectrum.h>

#include <Features/Model/SkyBox.h>
#include <Features/PostEffects/GrayScale.h>

#include <Features/TextRenderer/TextGenerator.h>
#include <Features/AudioSpectrum/SpectrumTextureGenerator.h>
#include <Features/UI/UISlider.h>

#include <Features/TextRenderer/Text3DRenderer.h>
#include <Features/TextRenderer/AtlasData.h>
class SampleScene : public BaseScene
{
public:

     ~SampleScene() override;

    void Initialize(SceneData* _sceneData) override;
    void Update() override;
    void Draw() override;
    void DrawShadow() override;

private:
    // -----------------------------
    // シーン関連 基本セット

    // カメラ
    Camera SceneCamera_ = {};

    // デバッグカメラ
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    // ライン描画
    LineDrawer* lineDrawer_ = nullptr;
    // 入力
    Input* input_ = nullptr;
    // パーティクル
    ParticleSystem* particleManager_ = nullptr;
    // ライト
    std::shared_ptr<LightGroup> lights_;


    //------------------------------
    // シーン固有


    std::vector<std::unique_ptr<ObjectModel>> rings_;

    std::shared_ptr<SoundInstance> soundInstance_ = nullptr;
    std::shared_ptr<VoiceInstance> voiceInstance_ = nullptr;

    // テキストジェネレータ
    TextGenerator textGenerator_;

    AudioSpectrum audioSpectrum_; 

    std::unique_ptr<SpectrumTextureGenerator> textureGenerator_;
    Text3DRenderer* text3DRenderer_ = nullptr;
    AtlasData* fontAtlas_ = nullptr;

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};
