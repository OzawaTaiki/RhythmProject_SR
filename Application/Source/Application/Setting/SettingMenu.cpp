#include "SettingMenu.h"

#include <System/Input/Input.h>
#include <Debug/Debug.h>
#include <System/Audio/AudioSystem.h>
#include <Features/Event/EventManager.h>
#include <Features/UI/UINavigationManager.h>

#include <Application/Setting/Setting.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UI/Element/UITextElement.h>

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
    backSprite_ = std::make_unique<UIImageElement>("SettingMenu_Fullback", Vector2(100, 100), Vector2(600, 400));
    backSprite_->Initialize();

    auto back = std::make_unique<UIImageElement>("SettingMenu_back", Vector2(100, 100), Vector2(600, 400));
    back->Initialize();
    auto volumeSlider = std::make_unique<UISliderElement>("VolumeSlider", Vector2(150, 150), Vector2(200, 20),true);
    volumeSlider->Initialize();
    volumeSlider->SetStep(0.01f);
    volumeSlider->SetValue(Setting::current_.masterVolume);
    volumeSlider->SetOnValueChanged([](float value)
                                    {
                                        Setting::current_.masterVolume = value; // 音量を設定に反映
                                        AudioSystem::GetInstance()->SetMasterVolume(value); // マスター音量を設定
                                        Debug::Log(std::format("Volume changed: {}\n", value));
                                    });

    auto label = std::make_unique<UITextElement>("VolumeLabel", Vector2(100, 150),  "Volume");
    label->Initialize();

    volumeSlider->AddChild(std::move(label));


    auto noteSpeedSlider = std::make_unique<UISliderElement>("NoteSpeedSlider", Vector2(150, 200), Vector2(200, 20), true);
    noteSpeedSlider->Initialize();
    noteSpeedSlider->SetStep(5.0f);
    noteSpeedSlider->SetValue(Setting::current_.noteSpeed);
    noteSpeedSlider->SetOnValueChanged([&](float value)
                                       {
                                           Setting::current_.noteSpeed = value; // ノーツ速度を設定に反映
                                           ValueChangedEventData eventData("NoteSpeed", value);
                                           EventManager::GetInstance()->DispatchEvent(GameEvent("ValueChanged", &eventData)); // ノーツ速度変更イベントをディスパッチ
                                           Debug::Log(std::format("Note speed changed: {}\n", value));
                                       });

    label = std::make_unique<UITextElement>("NoteSpeedLabel", Vector2(100, 200),  "Speed");
    label->Initialize();
    noteSpeedSlider->AddChild(std::move(label));

    auto audioLatencySlider = std::make_unique<UISliderElement>("AudioLatencySlider", Vector2(150, 250), Vector2(200, 20), true);
    audioLatencySlider->Initialize();
    audioLatencySlider->SetValue(Setting::current_.audioLatencyMs);
    audioLatencySlider->SetStep(1.0f);
    audioLatencySlider->SetOnValueChanged([&](float value)
                                          {
                                              Setting::current_.audioLatencyMs = value; // 音声遅延を設定に反映
                                              ValueChangedEventData eventData("AudioLatency", value);
                                              EventManager::GetInstance()->DispatchEvent(GameEvent("ValueChanged", &eventData)); // 音声遅延変更イベントをディスパッチ
                                              Debug::Log(std::format("Audio latency changed: {}\n", value));
                                          });

    label = std::make_unique<UITextElement>("AudioLatencyLabel", Vector2(100, 250),  "Latency");
    label->Initialize();
    audioLatencySlider->AddChild(std::move(label));

    auto closeButton = std::make_unique<UIButtonElement>("SettingMenu_CloseButton", Vector2(350, 300), Vector2(100, 40));
    closeButton->Initialize();
    closeButton->SetOnClick([this]()
                               {
                                   isActive_ = false;
                                   UINavigationManager::GetInstance()->ClearFocus();
                                   EventManager::GetInstance()->DispatchEvent(GameEvent("CloseOptionMenu", nullptr));
                                   previewPanel_->StopMusic();
                                   Debug::Log("SettingMenu closed\n");
                               });
    auto closeLabel = std::make_unique<UITextElement>("SettingMenu_CloseLabel", Vector2(10, 10), "Close");
    closeLabel->Initialize();
    closeButton->AddChild(std::move(closeLabel));



    auto navi1 = volumeSlider->GetComponent<UINavigationComponent>();
    auto navi2 = noteSpeedSlider->GetComponent<UINavigationComponent>();
    auto navi3 = audioLatencySlider->GetComponent<UINavigationComponent>();
    auto closeNavi = closeButton->GetComponent<UINavigationComponent>();

    navi1->SetNavigation(NavigationDirection::Up, closeButton.get());
    navi1->SetNavigation(NavigationDirection::Down, noteSpeedSlider.get());

    navi2->SetNavigation(NavigationDirection::Up, volumeSlider.get());
    navi2->SetNavigation(NavigationDirection::Down, audioLatencySlider.get());
    navi3->SetNavigation(NavigationDirection::Up, noteSpeedSlider.get());
    navi3->SetNavigation(NavigationDirection::Down, closeButton.get());

    closeNavi->SetNavigation(NavigationDirection::Up, audioLatencySlider.get());
    closeNavi->SetNavigation(NavigationDirection::Down, volumeSlider.get());

    volumeSlider_= back->AddChild(std::move(volumeSlider));
    back->AddChild(std::move(noteSpeedSlider));
    back->AddChild(std::move(audioLatencySlider));
    back->AddChild(std::move(closeButton));

    backSprite_->AddChild(std::move(back));


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
        UINavigationManager::GetInstance()->ClearFocus();
        EventManager::GetInstance()->DispatchEvent(GameEvent("CloseOptionMenu", nullptr));
        previewPanel_->StopMusic();
        Debug::Log("SettingMenu closed\n");
    }

    backSprite_->Update();
    previewPanel_->Update();

}

void SettingMenu::Draw()
{
    if (!isActive_)
        return;

    backSprite_->Draw();
    previewPanel_->Draw();
}

void SettingMenu::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "OpenOptionMenu")
    {
        isActive_ = true;
        UINavigationManager::GetInstance()->SetFocus(volumeSlider_);
        Debug::Log("SettingMenu opened\n");
    }
}
