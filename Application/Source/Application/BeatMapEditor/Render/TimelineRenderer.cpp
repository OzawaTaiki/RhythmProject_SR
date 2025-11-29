#include "TimelineRenderer.h"
#include <Application/BeatMapEditor/AudioController.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>

namespace BME {

void TimelineRenderer::Initialize()
{
    // TODO: タイムライン初期化

    timelineSprites_.clear();
    timelineSprites_["background"] = std::make_unique<UISprite>();
    timelineSprites_["background"]->Initialize("TimelineBackgroundSprite");

    timelineSprites_["start"] = std::make_unique<UISprite>();
    timelineSprites_["start"]->Initialize("TimelineStartSprite");

    timelineSprites_["end"] = std::make_unique<UISprite>();
    timelineSprites_["end"]->Initialize("TimelineEndSprite");

    timelineSprites_["playhead"] = std::make_unique<UISprite>();
    timelineSprites_["playhead"]->Initialize("TimelinePlayheadSprite");

    timelineSprites_["toTestButton"] = std::make_unique<UISprite>();
    timelineSprites_["toTestButton"]->Initialize("ToTestButtonSprite");

    timelineStartPosition_ = timelineSprites_["start"]->GetPos().x; // タイムラインの開始位置を取得
    timelineEndPosition_ = timelineSprites_["end"]->GetPos().x; // タイムラインの終了位置を取得
    timelineWidth_ = timelineEndPosition_ - timelineStartPosition_; // タイムラインの幅を計算

    dummy_timeline_ = std::make_unique<UISprite>();
    dummy_timeline_->Initialize("DummyTimelineSprite");
    dummy_timeline_->SetPos(Vector2(timelineStartPosition_, timelineSprites_["background"]->GetPos().y)); // タイムラインの開始位置を設定
    dummy_timeline_->SetAnchor(Vector2(0.0f, 0.0f)); // ダミータイムラインのアンカーを左上に設定
    dummy_timeline_->SetSize(Vector2(timelineWidth_, timelineSprites_["background"]->GetSize().y)); // ダミータイムラインのサイズを設定

    textParam_.SetPosition(timelineSprites_["toTestButton"]->GetPos())
        .SetPivot(timelineSprites_["toTestButton"]->GetAnchor())
        .SetColor({ 0,0,0,1 });
}

void TimelineRenderer::Draw(const AudioController* _audioController, float _currentTime)
{
    DataUpdate(_audioController, _currentTime);

    LineDrawer::GetInstance()->RegisterPoint(timelineSprites_["start"]->GetPos(), timelineSprites_["end"]->GetPos(), {1,1,1,1});

    timelineSprites_["background"]->Draw();
    timelineSprites_["start"]->Draw();
    timelineSprites_["end"]->Draw();
    timelineSprites_["playhead"]->Draw();
    timelineSprites_["toTestButton"]->Draw();
    //text_.Draw(L"テスト", textParam_);

    //if (!currentBeatMapData_.notes.empty())
    //{
    //    if (timelineSprites_["toTestButton"]->IsPointInside(input->GetMousePosition()) && input->IsMouseTriggered(0))
    //    {
    //        toTest_ = true;
    //    }
    //}
}

void TimelineRenderer::Finalize()
{
    // TODO: 終了処理
}

void TimelineRenderer::DataUpdate(const AudioController* _audioController, float _currentTime)
{
    if (!_audioController)
        return; // 音楽がロードされていない場合は何もしない

    auto musicSoundInstance = _audioController->GetSoundInstance();
    if (!musicSoundInstance)
        return;

    auto input = Input::GetInstance();

    if (dummy_timeline_->IsPointInside(input->GetMousePosition()) && input->IsMousePressed(0))
    {
        Vector2 mousePos = input->GetMousePosition();

        // マウス座標をタイムラインの相対位置に変換 時間軸状の座標
        float relativeX = mousePos.x - timelineStartPosition_;
        float ratio = relativeX / timelineWidth_; // タイムラインの幅に対する比率を計算

        ratio = std::clamp(ratio, 0.0f, 1.0f); // 比率を0から1の範囲に制限

        float time = musicSoundInstance->GetDuration() * ratio; // 音楽の長さに基づいて時間を計算

        _currentTime = time; // 現在の時間を更新

        // playheadの座標を更新
        timelineSprites_["playhead"]->SetPos(Vector2(timelineStartPosition_ + relativeX, timelineSprites_["playhead"]->GetPos().y));
    }
    else
    {
        Vector2 updatePosition;
        updatePosition.y = timelineSprites_["playhead"]->GetPos().y; // Y座標は固定

        float ratio = _currentTime / musicSoundInstance->GetDuration(); // 現在の時間を音楽の長さで割って比率を計算
        ratio = std::clamp(ratio, 0.0f, 1.0f); // 比率を0から1の範囲に制限
        updatePosition.x = timelineStartPosition_ + ratio * timelineWidth_; // タイムラインの開始位置と幅を使ってX座標を計算

        timelineSprites_["playhead"]->SetPos(updatePosition); // playheadの位置を更新
    }
}

} // namespace BME
