#pragma once
#include <map>
#include <Features/UI/Element/UIImageElement.h>
#include <functional>
#include <Features/UI/Element/UIButtonElement.h>
#include <Features/UI/Collider/UIRecntangleCollider.h>

namespace BME
{

class AudioController;
class State;

/// <summary>
/// タイムライン描画クラス
/// </summary>
class TimelineRenderer
{
public:
    TimelineRenderer() = default;
    ~TimelineRenderer() = default;

    void Initialize(std::function<void()> toTestFunc);
    void ProcessTimeline(const AudioController* _audioController, float& _currentTime);
    void Finalize();

private:

    void DataUpdate(const AudioController* _audioController, float& _currentTime);

private:

    std::map<std::string, std::unique_ptr<UIElement>> timelineSprites_;
    Rect dummy_timeline_;
    float timelineStartPosition_ = 0.0f; // タイムラインの開始位置
    float timelineEndPosition_ = 0.0f; // タイムラインの終了位置
    float timelineWidth_ = 0.0f; // タイムラインの幅

    bool toTest_ = false; // テストモードへ移行フラグ
    TextParam textParam_;
};

} // namespace BME
