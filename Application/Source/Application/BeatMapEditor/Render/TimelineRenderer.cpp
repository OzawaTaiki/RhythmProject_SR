#include "TimelineRenderer.h"
#include <Application/BeatMapEditor/AudioController.h>
#include <Application/BeatMapEditor/EditorState.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>

using namespace Engine;


namespace BME
{

void TimelineRenderer::Initialize(std::function<void()> toTestFunc)
{
    // TODO: タイムライン初期化

    timelineSprites_.clear();
    timelineSprites_["background"] = std::make_unique<UIImageElement>("TimelineBackgroundSprite", Vector2(0, 0), Vector2(800, 50));
    timelineSprites_["background"]->Initialize();

    timelineSprites_["start"] = std::make_unique<UIImageElement>("TimelineStartSprite", Vector2(0, 0), Vector2(20, 50));
    timelineSprites_["start"]->Initialize();

    timelineSprites_["end"] = std::make_unique<UIImageElement>("TimelineEndSprite", Vector2(780, 0), Vector2(20, 50));
    timelineSprites_["end"]->Initialize();

    timelineSprites_["playhead"] = std::make_unique<UIImageElement>("TimelinePlayheadSprite", Vector2(0, 0), Vector2(10, 60));
    timelineSprites_["playhead"]->Initialize();

    auto button = std::make_unique<UIButtonElement>("TimelineToTestButtonSprite", Vector2(820, 0), Vector2(100, 50), "To Test");
    button->Initialize();
    button->SetOnClickUp([toTestFunc]()
                         {
                             toTestFunc();
                         });
    timelineSprites_["toTestButton"] = std::move(button);
    timelineStartPosition_ = timelineSprites_["start"]->GetPosition().x; // タイムラインの開始位置を取得
    timelineEndPosition_ = timelineSprites_["end"]->GetPosition().x; // タイムラインの終了位置を取得
    timelineWidth_ = timelineEndPosition_ - timelineStartPosition_; // タイムラインの幅を計算
    timelineSprites_["playhead"]->SetPosition(Vector2(timelineStartPosition_, timelineSprites_["playhead"]->GetPosition().y));

    dummy_timeline_ = Rect();
    dummy_timeline_.leftTop = Vector2(timelineStartPosition_, timelineSprites_["background"]->GetPosition().y);
    dummy_timeline_.size = Vector2(timelineWidth_, timelineSprites_["background"]->GetSize().y);


    textParam_.SetPosition(timelineSprites_["toTestButton"]->GetPosition())
        .SetPivot(timelineSprites_["toTestButton"]->GetAnchor())
        .SetColor({ 0,0,0,1 });
}

void TimelineRenderer::ProcessTimeline(const AudioController* _audioController, float& _currentTime)
{
    DataUpdate(_audioController, _currentTime);

    LineDrawer::GetInstance()->RegisterPoint(timelineSprites_["start"]->GetPosition(), timelineSprites_["end"]->GetPosition(), { 1,1,1,1 });

    timelineSprites_["background"]  ->Draw();
    timelineSprites_["start"]       ->Draw();
    timelineSprites_["end"]         ->Draw();
    timelineSprites_["playhead"]    ->Draw();
    timelineSprites_["toTestButton"]->Draw();
    //text_.Draw(L"テスト", textParam_);

   /* auto input = Input::GetInstance();
    if (timelineSprites_["toTestButton"]->IsPointInside(input->GetMousePosition()) && input->IsMouseTriggered(0))
    {
        state->SetToTestMode(true);
    }*/
}

void TimelineRenderer::Finalize()
{
    // TODO: 終了処理
}

void TimelineRenderer::DataUpdate(const AudioController* _audioController, float& _currentTime)
{
    if (!_audioController)
        return; // 音楽がロードされていない場合は何もしない

    auto musicSoundInstance = _audioController->GetSoundInstance();
    if (musicSoundInstance)
    {
        auto input = Input::GetInstance();

        if (dummy_timeline_.Contains(input->GetMousePosition()) && input->IsMousePressed(0))
        {
            Vector2 mousePos = input->GetMousePosition();

            // マウス座標をタイムラインの相対位置に変換 時間軸状の座標
            float relativeX = mousePos.x - timelineStartPosition_;
            float ratio = relativeX / timelineWidth_; // タイムラインの幅に対する比率を計算

            ratio = std::clamp(ratio, 0.0f, 1.0f); // 比率を0から1の範囲に制限

            float time = musicSoundInstance->GetDuration() * ratio; // 音楽の長さに基づいて時間を計算

            _currentTime = time; // 現在の時間を更新

            // playheadの座標を更新
            timelineSprites_["playhead"]->SetPosition(Vector2(timelineStartPosition_ + relativeX, timelineSprites_["playhead"]->GetPosition().y));
        }
        else
        {
            Vector2 updatePosition;
            updatePosition.y = timelineSprites_["playhead"]->GetPosition().y; // Y座標は固定

            float ratio = _currentTime / musicSoundInstance->GetDuration(); // 現在の時間を音楽の長さで割って比率を計算
            ratio = std::clamp(ratio, 0.0f, 1.0f); // 比率を0から1の範囲に制限
            updatePosition.x = timelineStartPosition_ + ratio * timelineWidth_; // タイムラインの開始位置と幅を使ってX座標を計算

            timelineSprites_["playhead"]->SetPosition(updatePosition); // playheadの位置を更新
        }
    }

    timelineSprites_["background"]  ->Update();
    timelineSprites_["start"]       ->Update();
    timelineSprites_["end"]         ->Update();
    timelineSprites_["playhead"]    ->Update();
    timelineSprites_["toTestButton"]->Update();

}

} // namespace BME
