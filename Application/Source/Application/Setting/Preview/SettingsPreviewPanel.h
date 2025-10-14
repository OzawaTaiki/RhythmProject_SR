#pragma once
#include <memory>

#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <Features/UI/UIButton.h>
#include <Features/UI/UISprite.h>


#include <Core/DXCommon/RTV/RenderTexture.h>

#include <Application/Lane/Lane.h>

class Camera;

class SettingsPreviewPanel
{
public:
    SettingsPreviewPanel() = default;
    ~SettingsPreviewPanel() = default;

    void Initialize();
    void Update();
    void Draw();

private:

    std::vector<std::unique_ptr<Lane>> lanes_;

    RenderTarget* renderTexture_;

    std::shared_ptr<SoundInstance> soundInstance_ = nullptr;
    std::shared_ptr<VoiceInstance> voiceInstance_ = nullptr;

    std::shared_ptr<UIButton> musicToggleButton_ = nullptr;
    std::shared_ptr<UISprite> previewSprite_ = nullptr;
    uint32_t previewTextureHandle_ = 0;
    Camera camera_ = {};
        

};