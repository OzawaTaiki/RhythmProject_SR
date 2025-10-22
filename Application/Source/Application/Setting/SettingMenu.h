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

// 設定メニュークラス
class SettingMenu : public iEventListener
{
public:
    SettingMenu();
    ~SettingMenu();
    // 初期化
    void Initialize();
    // 更新
    void Update();
    // 描画
    void Draw();
    // メニューがアクティブかどうか
    bool IsActive() const { return isActive_; }

    // UIグループを取得
    UIGroup* GetUIGroup() const { return uiGroup_.get(); }
    // イベント受信
    void OnEvent(const GameEvent& _event) override;

private:

    bool isActive_ = false; // メニューがアクティブかどうか

    std::unique_ptr<UIGroup> uiGroup_ = nullptr; // UIグループ

    std::function<void(float)> speedSetFunc_ = nullptr; // ノーツ速度を設定するコールバック関数
    std::function<void(float)> audioLatencySetFunc_ = nullptr; // 音声遅延を設定するコールバック関数

    std::vector<std::shared_ptr<UISlider>> sliders_; // スライダー   のリスト

    std::unique_ptr<SettingsPreviewPanel> previewPanel_ = nullptr; // 設定プレビュー
};