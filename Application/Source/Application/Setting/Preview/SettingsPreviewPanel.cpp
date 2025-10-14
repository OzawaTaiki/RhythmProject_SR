#include "SettingsPreviewPanel.h"

#include <System/Audio/AudioSystem.h>
#include <Features/Camera/Camera/Camera.h>

#include <Application/Setting/Setting.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Core/DXCommon/RTV/RTVManager.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <System/Input/Input.h>


void SettingsPreviewPanel::Initialize()
{
    camera_.Initialize(CameraType::Perspective, Vector2(320, 720));
    camera_.aspectRatio_ = 0.7f;
    camera_.farClip_ = 100.0f;
    camera_.translate_= { 0.0f,5.0f,-13.0f };
    camera_.rotate_.x = 0.26f;
    camera_.Update();
    camera_.UpdateMatrix();

    const float bpm = 120.0f;
    const float spb = 60.0f / bpm * 4; // 秒/拍
    // 4分音符の間隔でノーツを生成
    std::list<NoteData> notes;
    const size_t numNotes = 16; // 16個のノーツを生成
    for (size_t i = 0; i < numNotes; ++i)
    {
        NoteData note;
        note.laneIndex = i % 2 + 1; // 1 or 2
        note.targetTime = i * spb;
        note.noteType = "normal";
        note.holdDuration = 0.0f;

        notes.push_back(note);
    }

    // TODO : れーんを4つ それとノーツをいくつか生成
    for (int i = 0; i < 4; ++i)
    {
        auto lane = std::make_unique<Lane>();
        lane->Initialize(notes, i, 0.0f, Setting::current_.noteSpeed, 0.0f);
        lanes_.push_back(std::move(lane));
    }

    musicToggleButton_ = std::make_shared<UIButton>();
    musicToggleButton_->Initialize("MusicToggleButton", L"Play");
    musicToggleButton_->SetOnClick([&]()
                                   {
                                       if (voiceInstance_ && voiceInstance_->IsPlaying())
                                       {
                                           voiceInstance_->Stop();
                                           musicToggleButton_->SetText(L"Play");
                                       }
                                       else
                                       {
                                           voiceInstance_ = soundInstance_->Play(0.5f);//  masterを調整しているから音源ごとのvolumeは固定
                                           musicToggleButton_->SetText(L"Stop");
                                       }
                                   });


    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/120bpm_16beats.wav");

    // レンダーターゲットの作成
    RTVManager::GetInstance()->CreateRenderTarget(
        "SettingsPreviewPanelRT",
        320, 720,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        { 0.0f,0.0f ,0.0f ,1.0f },
        true);
    // 取得
    renderTexture_ = RTVManager::GetInstance()->GetRenderTexture("SettingsPreviewPanelRT");
    previewTextureHandle_= TextureManager::GetInstance()->GetTextureHandle(
        "SettingsPreviewPanelRT",
        renderTexture_->GetSRVIndex(),
        renderTexture_->GetGPUHandleofRTV());

    previewSprite_ = std::make_shared<UISprite>();
    previewSprite_->Initialize("SettingsPreviewPanel");
    previewSprite_->SetTextureHandle(previewTextureHandle_);

};

void SettingsPreviewPanel::Update()
{
    if(Input::GetInstance()->IsKeyTriggered(DIK_SPACE))
    {
        if (voiceInstance_ && voiceInstance_->IsPlaying())
        {
            voiceInstance_->Stop();
            musicToggleButton_->SetText(L"Play");
        }
        else
        {
            voiceInstance_ = soundInstance_->Play(Setting::current_.masterVolume* 0.5f, true);//  masterを調整しているから音源ごとのvolumeは固定
            musicToggleButton_->SetText(L"Stop");
        }
    }

    bool enableMusic = (voiceInstance_ && voiceInstance_->IsPlaying());
    float elapsedTime = enableMusic ? voiceInstance_->GetElapsedTime() : 0.0f;
    elapsedTime = std::fmod(elapsedTime, soundInstance_->GetDuration()); // 楽曲時間でループ
    for (auto& lane : lanes_)
    {
        lane->Update(elapsedTime, Setting::current_.noteSpeed);
    }


    musicToggleButton_->Update();

    camera_.ImGui();
}

void SettingsPreviewPanel::Draw()
{
    renderTexture_->SetRenderTexture();

    ModelManager::GetInstance()->PreDrawForObjectModel();
    for (auto& lane : lanes_)
        lane->Draw(&camera_);
    renderTexture_->ChangeRTVState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    LayerSystem::SetLayer("PauseMenu");
    previewSprite_->Draw();

    //musicToggleButton_->Draw();
}
