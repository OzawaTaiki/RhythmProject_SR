#pragma once
#include <memory>

#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <Features/UI/Element/UIButtonElement.h>
#include <Features/UI/Element/UIImageElement.h>


#include <Core/DXCommon/RTV/RenderTexture.h>

#include <Application/Lane/Lane.h>

// 前方宣言
namespace Engine
{
class Camera;
}

// 設定プレビュー用パネルクラス
/// <summary>
/// 設定画面のプレビュー表示を行うパネルクラス。ノーツや音声のプレビューを描画する。
/// </summary>
class SettingsPreviewPanel
{
public:
    SettingsPreviewPanel() = default;
    ~SettingsPreviewPanel() = default;

    void Initialize();
    void Update();
    void Draw();

    void StopMusic();
private:

    std::vector<std::unique_ptr<Lane>> lanes_; // レーンのリスト

    Engine::RenderTarget* renderTexture_; // 専用描画テクスチャ

    std::shared_ptr<Engine::SoundInstance> soundInstance_ = nullptr; // メトロノーム音源
    std::shared_ptr<Engine::VoiceInstance> voiceInstance_ = nullptr; // 再生中の音源

    std::shared_ptr<Engine::UIButtonElement> musicToggleButton_ = nullptr; // 音楽再生トグルボタン
    std::shared_ptr<Engine::UIImageElement> previewSprite_ = nullptr; // プレビュー表示用スプライト
    uint32_t previewTextureHandle_ = 0; // プレビューテクスチャハンドル
    Engine::Camera camera_ = {}; // プレビュー用カメラ
};