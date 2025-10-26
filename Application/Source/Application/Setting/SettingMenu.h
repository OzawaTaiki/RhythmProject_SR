#pragma once

#include <Features/UI/UIGroup.h>
#include <Features/Event/EventData.h>
#include <Features/UI/UISlider.h>
#include <Features/Event/EventListener.h>
#include <Application/Setting/Preview/SettingsPreviewPanel.h>

// スライダーの値が変更されたときのイベントデータ
struct ValueChangedEventData : EventData
{
    ValueChangedEventData(const std::string& _name, float _value)
        : name(_name), value(_value) {}
    ~ValueChangedEventData() override = default;

    std::string name;   // スライダーの名前
    float value;        // スライダーの値
};

/// <summary>
/// 設定メニュークラス。
/// </summary>
class SettingMenu : public iEventListener
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
    /// 内部の UI グループを取得する。
    /// </summary>
    UIGroup* GetUIGroup() const { return uiGroup_.get(); }

    /// <summary>
    /// イベントを受信するハンドラ。
    /// </summary>
    void OnEvent(const GameEvent& _event) override;

private:

    bool isActive_ = false; // メニューがアクティブかどうか

    std::unique_ptr<UIGroup> uiGroup_ = nullptr; // UIグループ

    std::function<void(float)> speedSetFunc_ = nullptr; // ノーツ速度を設定するコールバック関数
    std::function<void(float)> audioLatencySetFunc_ = nullptr; // 音声遅延を設定するコールバック関数

    std::vector<std::shared_ptr<UISlider>> sliders_; // スライダー   のリスト

    std::unique_ptr<SettingsPreviewPanel> previewPanel_ = nullptr; // 設定プレビュー
};