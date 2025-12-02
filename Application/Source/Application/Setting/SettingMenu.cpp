#include "SettingMenu.h"

#include <System/Input/Input.h>
#include <Debug/Debug.h>
#include <System/Audio/AudioSystem.h>
#include <Features/Event/EventManager.h>

#include <Application/Setting/Setting.h>

SettingMenu::SettingMenu()
{
    // イベントリスナーとして登録
    EventManager::GetInstance()->AddEventListener("OpenOptionMenu", this);
}

SettingMenu::~SettingMenu()
{
    // イベントリスナーから登録解除
    EventManager::GetInstance()->RemoveEventListener("OpenOptionMenu", this);
}

void SettingMenu::Initialize()
{
    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    // 背景スプライト作成
    auto sprite = uiGroup_->CreateSprite("SettingMenu_back", L"Settings");

    // 音量スライダー作成
    auto volumeSlider = uiGroup_->CreateSlider("VolumeSlider", 0.0f, 1.0f);
    volumeSlider->SetSize({ 200, 20 });
    volumeSlider->SetValue(Setting::current_.masterVolume);
    volumeSlider->SetOnValueChanged([](float value)
                                    {
                                        Setting::current_.masterVolume = value; // 音量を設定に反映
                                        AudioSystem::GetInstance()->SetMasterVolume(value); // マスター音量を設定
                                        Debug::Log(std::format("Volume changed: {}\n", value));
                                    });

    // ノーツ速度スライダー作成
    auto noteSpeedSlider = uiGroup_->CreateSlider("NoteSpeedSlider", 5.0f, 100.0f);
    //noteSpeedSlider->SetPos({ 100, 150 });
    noteSpeedSlider->SetSize({ 200, 20 });
    noteSpeedSlider->SetValue(Setting::current_.noteSpeed);
    noteSpeedSlider->SetOnValueChanged([&](float value)
                                       {
                                           Setting::current_.noteSpeed = value; // ノーツ速度を設定に反映

                                           ValueChangedEventData eventData("NoteSpeed", value);
                                           EventManager::GetInstance()->DispatchEvent(GameEvent("ValueChanged", &eventData)); // ノーツ速度変更イベントをディスパッチ
                                           Debug::Log(std::format("Note speed changed: {}\n", value));
                                       });

    // 音声遅延スライダー作成
    auto audioLatencySlider = uiGroup_->CreateSlider("AudioLatencySlider", -1000.0f, 1000.0f);
    audioLatencySlider->SetSize({ 200, 20 });
    audioLatencySlider->SetValue(Setting::current_.audioLatencyMs);
    audioLatencySlider->SetOnValueChanged([&](float value)
                                          {
                                              Setting::current_.audioLatencyMs = value; // 音声遅延を設定に   反映

                                              ValueChangedEventData eventData("AudioLatency", value);
                                              EventManager::GetInstance()->DispatchEvent(GameEvent("ValueChanged", &eventData)); // 音声遅延変更イベントをディスパッチ
                                              Debug::Log(std::format("Audio latency changed: {}\n", value));
                                          });

    // スプライトにスライダーを追加
    sprite->AddChild(volumeSlider);
    sprite->AddChild(noteSpeedSlider);
    sprite->AddChild(audioLatencySlider);

    // スライダーをリストに追加
    sliders_.push_back(volumeSlider);
    sliders_.push_back(noteSpeedSlider);
    sliders_.push_back(audioLatencySlider);

    previewPanel_ = std::make_unique<SettingsPreviewPanel>();
    previewPanel_->Initialize();

    isActive_ = false;
}

void SettingMenu::Update()
{
    if (!isActive_)
        return;

    // ESCキーでメニューを閉じる
    if (Input::GetInstance()->IsKeyTriggered(DIK_ESCAPE))
    {
        isActive_ = false;
        EventManager::GetInstance()->DispatchEvent(GameEvent("CloseOptionMenu", nullptr));
        previewPanel_->StopMusic();
        Debug::Log("SettingMenu closed\n");
    }

    uiGroup_->Update();
    previewPanel_->Update();

}

void SettingMenu::Draw()
{
    if (!isActive_)
        return;

    uiGroup_->Draw();
    previewPanel_->Draw();
}

void SettingMenu::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "OpenOptionMenu")
    {
        isActive_ = true;
        Debug::Log("SettingMenu opened\n");
    }
}
