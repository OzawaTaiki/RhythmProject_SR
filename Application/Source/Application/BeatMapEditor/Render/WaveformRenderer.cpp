#include "WaveformRenderer.h"
#include <Application/BeatMapEditor/AudioController.h>
#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <Core/WinApp/WinApp.h>

namespace BME {

void WaveformRenderer::Initialize(const Matrix4x4& _matVP)
{
    // 波形表示範囲を設定（固定値）
    // TODO: EditorCoordinateから動的に計算する方が良い
    WaveformBounds bounds(Vector2(300.0f, 0.0f), Vector2(WinApp::kWindowSize_.x - 600.0f, 96.0f));

    // WaveformDisplayを初期化
    // SoundInstanceはnullptrで初期化し、Draw時に設定
    waveformDisplay_.Initialize(nullptr, bounds, _matVP);

    waveformBackground_ = std::make_unique<Sprite>("waveformBackground", false);
    waveformBackground_->Initialize();
    waveformBackground_->SetAnchor(Vector2(0.0f, 0.0f)); // アンカーを左上に設定
    waveformBackground_->translate_ = bounds.leftTop;
    waveformBackground_->SetSize(bounds.size);
}


void WaveformRenderer::Draw(const AudioController* audioController, float currentTime)
{
    if (!audioController || !audioController->HasAudio())
        return;
    //
    waveformDisplay_.SetSoundInstance(audioController->GetSoundInstance());

    // 現在時刻を設定
    waveformDisplay_.SetStartTime(currentTime);

    // 波形描画
    waveformDisplay_.Draw();
}

void WaveformRenderer::DrawBackSprite()
{// レイヤーの影響を受けるため別関数で用意
    waveformBackground_->Draw(Vector4(0, 0, 0, 1));
}

void WaveformRenderer::Finalize()
{
    // 特に終了処理不要
}

void WaveformRenderer::UpdateBounds()
{
    // 表示範囲を更新（将来的にリサイズ対応）
    WaveformBounds bounds(Vector2(300.0f, 0.0f), Vector2(WinApp::kWindowSize_.x - 600.0f, 96.0f));
    waveformDisplay_.SetBounds(bounds);
}

} // namespace BME
