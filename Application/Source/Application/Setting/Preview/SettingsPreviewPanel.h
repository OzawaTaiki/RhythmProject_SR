#pragma once
#include <memory>

#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <Features/UI/UIButton.h>
#include <Features/UI/UISprite.h>


#include <Core/DXCommon/RTV/RenderTexture.h>

#include <Application/Lane/Lane.h>

// 前方宣言
class Camera;

// 設定プレビュー用パネルクラス
class SettingsPreviewPanel
{
public:
    SettingsPreviewPanel() = default;
    ~SettingsPreviewPanel() = default;

    void Initialize();
    void Update();
    void Draw();

private:

    std::vector<std::unique_ptr<Lane>> lanes_; // レーンのリスト

    RenderTarget* renderTexture_; // 専用描画テクスチャ

    std::shared_ptr<SoundInstance> soundInstance_ = nullptr; // メトロノーム音源
    std::shared_ptr<VoiceInstance> voiceInstance_ = nullptr; // 再生中の音源

    std::shared_ptr<UIButton> musicToggleButton_ = nullptr; // 音楽再生トグルボタン
    std::shared_ptr<UISprite> previewSprite_ = nullptr; // プレビュー表示用スプライト
    uint32_t previewTextureHandle_ = 0; // プレビューテクスチャハンドル
    Camera camera_ = {}; // プレビュー用カメラ
};