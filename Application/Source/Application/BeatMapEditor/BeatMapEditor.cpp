#include "BeatMapEditor.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/Debug.h>
#include <Debug/ImGuiHelper.h>
#include <Debug/ImGuiDebugManager.h>
#include <System/Input/Input.h>
#include <System/Audio/AudioSystem.h>

#include <Application/BeatMapLoader/BeatMapLoader.h>
#include <Utility/FileDialog/FileDialog.h>
#include <Utility/StringUtils/StringUitls.h>

#include <Application/BeatMapEditor/Command/PlaceNoteCommand.h>
#include <Application/BeatMapEditor/Command/DeleteNoteCommand.h>
#include <Application/BeatMapEditor/Command/MoveNoteCommand.h>
#include <Application/BeatMapEditor/Command/ChangeHoldDurationCommand.h>
#include <Application/BeatMapEditor/Command/PasteCommand.h>


#include <fstream>
#include <Framework/LayerSystem/LayerSystem.h>

// TODO いろいろ
/// ブリッジに重ねてノーツを置けてしまう
/// zoom スクロール grid分割 全部キーボードあるいはマウスでできるように
///     Undo実装したい
///     BPM調整するやつ
//      流しながらノート入力したーい
//      targetTimeをもとに再生を行う

BeatMapEditor::BeatMapEditor()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->HideAllWindow();
#endif
}

BeatMapEditor::~BeatMapEditor()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->ShowAllWindow();
#endif
}

void BeatMapEditor::Initialize(const BeatMapData& _beatMapData)
{

    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr2D(&for2dCamera_); // 2Dカメラをライン描画クラスに設定

    for2dCamera_.Initialize(CameraType::Orthographic, WinApp::kWindowSize_); // 2Dカメラの初期化
    for2dCamera_.matProjection_ = Matrix4x4::Identity();
    for2dCamera_.matView_ = Matrix4x4::Identity();
    for2dCamera_.UpdateMatrix(); // カメラの行列を更新

    //                                       左右UI   マージン
    Vector2 laneAreaSize = Vector2(WinApp::kWindowSize_.x - 600.0f - 120.0f, WinApp::kWindowSize_.y); // レーンエリアのサイズを設定
    editorCoordinate_.Initialize(laneAreaSize); // 初期画面サイズとレーン数を設定
    editorCoordinate_.SetTimeZeroOffsetRatio(0.1f);

    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(120.0f, 0.0f); // 初期BPMとオフセットを設定
    // ========================================
    // 新アーキテクチャクラスの初期化
    // ========================================
    document_ = std::make_unique<BME::Document>();
    state_ = std::make_unique<BME::State>();
    audioController_ = std::make_unique<BME::AudioController>();
    inputHandler_ = std::make_unique<BME::InputHandler>();
    renderer_ = std::make_unique<BME::EditorRenderer>();
    fileManager_ = std::make_unique<BME::FileManager>();

    // レンダラー初期化
    renderer_->Initialize(&editorCoordinate_, for2dCamera_.GetViewProjection());

    // 初期譜面データを新Documentに設定
    document_->SetData(_beatMapData);
}

void BeatMapEditor::Update()
{
    auto voice = audioController_->GetVoiceInstance();
    if (voice && voice->IsPlaying())
        currentTime_ = voice->GetElapsedTime();

    editorCoordinate_.SetScrollOffset(currentTime_);

    inputHandler_->HandleInput(
        state_.get(),
        document_.get(),
        audioController_.get(),
        &commandHistory_,
        &editorCoordinate_,
        currentTime_
    );

}

void BeatMapEditor::Draw()
{
    // エディターの描画処理

    Sprite::PreDraw();

    renderer_->Draw(
        state_.get(),
        document_.get(),
        fileManager_.get(),
        audioController_.get(),
        &editorCoordinate_,
        beatManager_.get(),
        currentTime_
    );

}


void BeatMapEditor::SetNoteDuration(size_t _noteIndex, float _newDuration)
{
    if (_noteIndex >= currentBeatMapData_.notes.size())
    {
        Debug::Log("Invalid note index: " + std::to_string(_noteIndex) + "\n");
        return; // 無効なインデックスの場合は何もしない
    }
    if (currentBeatMapData_.notes[_noteIndex].noteType != "hold")
    {
        Debug::Log("Cannot set duration for non-hold note at index: " + std::to_string(_noteIndex) + "\n");
        return; // ホールドノート以外は変更できない
    }
    const auto& note = currentBeatMapData_.notes[_noteIndex];

    if (_newDuration <= 0.0f)
        return;

    float targetTime = note.targetTime + _newDuration;

    // グリッドスナップ
    if (gridSnapEnabled_)
    {
        _newDuration = editorCoordinate_.SnapTimeToGrid(targetTime - note.targetTime, currentBeatMapData_.bpm, static_cast<int>(1.0f / snapInterval_));
        if (_newDuration <= 0.0f)
            return;
    }
    currentBeatMapData_.notes[_noteIndex].holdDuration = _newDuration; // ノートの持続時間を更新
    isModified_ = true; // 譜面が変更されたフラグを立てる
}
