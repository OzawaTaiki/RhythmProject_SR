#pragma once

#include <Features/Event/EventData.h>

#include <Features/UI/Element/UIElement.h>
#include <Features/UI/Element/UISliderElement.h>
#include <Features/Event/EventListener.h>
#include <Application/Setting/Preview/SettingsPreviewPanel.h>

// スライダーの値が変更されたときのイベントデータ
struct ValueChangedEventData : Engine::EventData
{
    ValueChangedEventData(const std::string& name, float value)
        : name(name), value(value) {}
    ~ValueChangedEventData() override = default;

    std::string name;   // スライダーの名前
    float value;        // スライダーの値
};

/// <summary>
/// 設定メニュークラス。
/// </summary>
class SettingMenu : public Engine::iEventListener
{
public:
    SettingMenu();
    ~SettingMenu();

    /// <summary>
    /// 初期化処理。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update();

    /// <summary>
    /// UI の描画処理。
    /// </summary>
    void Draw();

    /// <summary>
    /// メニューがアクティブかどうかを返す。
    /// </summary>
    bool IsActive() const { return isActive_; }

    /// <summary>
    /// イベントを受信するハンドラ。
    /// </summary>
    void OnEvent(const Engine::GameEvent& event) override;

private:

    bool isActive_ = false; // メニューがアクティブかどうか

    std::unique_ptr<Engine::UIElement> backSprite_;
    Engine::UIElement* volumeSlider_ = nullptr;       // 音量スライダー フォーカスセット用
    std::function<void(float)> speedSetFunc_ = nullptr; // ノーツ速度を設定するコールバック関数
    std::function<void(float)> audioLatencySetFunc_ = nullptr; // 音声遅延を設定するコールバック関数

    std::unique_ptr<SettingsPreviewPanel> previewPanel_ = nullptr; // 設定プレビュー
};
