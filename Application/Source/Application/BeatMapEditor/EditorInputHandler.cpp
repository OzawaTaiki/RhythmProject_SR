#include "EditorInputHandler.h"

#include <Application/BeatMapEditor/BeatMapDocument.h>
#include <Application/BeatMapEditor/AudioController.h>
#include <Application/BeatMapEditor/Command/CommandHistory.h>
#include <Application/BeatMapEditor/Command/PlaceNoteCommand.h>
#include <Application/BeatMapEditor/Command/MoveNoteCommand.h>
#include <Application/BeatMapEditor/Command/DeleteNoteCommand.h>
#include <Application/BeatMapEditor/Command/PasteCommand.h>

#include <System/Input/Input.h>
#include <Debug/Debug.h>

namespace BME
{

InputHandler::InputHandler()
{
    input_ = Input::GetInstance();
}

// ========================================
// メイン入力処理
// ========================================

void InputHandler::HandleInput(
    State* state,
    Document* document,
    AudioController* audioController,
    CommandHistory* commandHistory,
    EditorCoordinate* coordinate,
    float& currentTime)
{
    if (!state || !document || !audioController || !commandHistory || !coordinate)
        return;

    HandleGlobalInput(state, document, audioController, commandHistory, currentTime);
    HandleModeSpecificInput(state, document, audioController, commandHistory, coordinate);
    HandleMouseWheelInput(coordinate, audioController, currentTime);
}

// ========================================
// グローバル入力（全モード共通）
// ========================================

void InputHandler::HandleGlobalInput(
    State* state,
    Document* document,
    AudioController* audioController,
    CommandHistory* commandHistory,
    float& currentTime)
{
    // Space: 音楽の再生/停止
    if (input_->IsKeyTriggered(DIK_SPACE) && state->GetCurrentMode() != EditorMode::BPMSetting)
    {
        if (audioController->IsPlaying())
            audioController->Stop();
        else
            audioController->Play(currentTime);
    }

    // ESC: 選択をクリア
    if (input_->IsKeyTriggered(DIK_ESCAPE))
    {
        state->ClearSelection();
    }

    // Delete: 選択ノートを削除
    if (input_->IsKeyTriggered(DIK_DELETE))
    {
        const auto& selectedIndices = state->GetSelectedNotes();
        if (!selectedIndices.empty())
        {
            // TODO:OK DeleteNoteCommandを実装
            // TODO: 選択ノート削除処理実装
            auto command = std::make_unique<DeleteNoteCommand>(document, selectedIndices);
            commandHistory->ExecuteCommand(std::move(command));
            state->ClearSelection();
        }
    }

    // Ctrl + Z: アンドゥ
    if (input_->IsKeyPressed(DIK_LCONTROL) && input_->IsKeyTriggered(DIK_Z))
    {
        commandHistory->Undo();
    }

    // Ctrl + Y: リドゥ
    if (input_->IsKeyPressed(DIK_LCONTROL) && input_->IsKeyTriggered(DIK_Y))
    {
        commandHistory->Redo();
    }

    // Ctrl + C: コピー
    if (input_->IsKeyPressed(DIK_LCONTROL) && input_->IsKeyTriggered(DIK_C))
    {
        // TODO:OK Copy実装
        state->Copy(document->GetData().notes, currentTime);
    }

    // Ctrl + V: ペースト
    if (input_->IsKeyPressed(DIK_LCONTROL) && input_->IsKeyTriggered(DIK_V))
    {
        // TODO: OK Paste実装
        // TODO: コマンドクラスの変更
        auto command = std::make_unique<PasteCommand>(document, state->GetPasteNotes(currentTime), 0.0f);
        commandHistory->ExecuteCommand(std::move(command));
    }

    // モード切り替え (1~5キー)
    if (input_->IsKeyTriggered(DIK_1))
        state->SetCurrentMode(EditorMode::Select);
    else if (input_->IsKeyTriggered(DIK_2))
        state->SetCurrentMode(EditorMode::PlaceNormalNote);
    else if (input_->IsKeyTriggered(DIK_3))
        state->SetCurrentMode(EditorMode::PlaceLongNote);
    else if (input_->IsKeyTriggered(DIK_4))
        state->SetCurrentMode(EditorMode::Delete);
    else if (input_->IsKeyTriggered(DIK_5))
        state->SetCurrentMode(EditorMode::LiveMapping);

    // Tab: モード循環
    if (input_->IsKeyTriggered(DIK_TAB))
    {
        int32_t mode = static_cast<int32_t>(state->GetCurrentMode());
        mode = (mode + 1) % static_cast<int32_t>(EditorMode::Count);
        state->SetCurrentMode(static_cast<EditorMode>(mode));
    }
}

// ========================================
// モード別入力
// ========================================

void InputHandler::HandleModeSpecificInput(
    State* state,
    Document* document,
    AudioController* audioController,
    CommandHistory* commandHistory,
    EditorCoordinate* coordinate)
{
    switch (state->GetCurrentMode())
    {
        case EditorMode::Select:
            HandleSelectMode(state, document, commandHistory, coordinate);
            break;
        case EditorMode::PlaceNormalNote:
            HandlePlaceNormalNoteMode(state, document, commandHistory, coordinate);
            break;
        case EditorMode::PlaceLongNote:
            HandlePlaceLongNoteMode(state, document, commandHistory, coordinate);
            break;
        case EditorMode::Delete:
            HandleDeleteMode(document, commandHistory, coordinate);
            break;
        case EditorMode::BPMSetting:
            HandleBPMSettingMode(state, audioController);
            break;
        default:
            break;
    }
}

// ========================================
// 選択モード
// ========================================

void InputHandler::HandleSelectMode(
    State* state,
    Document* document,
    CommandHistory* commandHistory,
    EditorCoordinate* coordinate    )
{
    const Vector2 mousePos = input_->GetMousePosition();

    // TODO:OK 選択モードの入力処理実装
    // - ノートのクリック選択
    {
        // マウス位置のノートを取得
        size_t noteIndex = GetNoteIndexFromMousePosition(document, coordinate);
        if (noteIndex != SIZE_MAX)
        {
            if (input_->IsMouseTriggered(0)) // 左クリックで選択
            {
                bool multiSelect = input_->IsKeyPressed(DIK_LCONTROL); // Ctrlキーでマルチセレクト
                state->SelectNote(static_cast<uint32_t>(noteIndex), multiSelect);

                float mouseTime = coordinate->ScreenYToTime(mousePos.y);
                mouseTime = SnapTimeToGrid(mouseTime, state, document->GetData());
                state->StartMove(document->GetData().notes, mouseTime);
            }
        }
        else
        {
            if (input_->IsMouseTriggered(0)) // 左クリックで選択
            {
                state->ClearSelection();
            }
        }
    }
    // - ドラッグによる範囲選択
    {
        if (!state->IsMovingSelectedNote()) // ノート移動中でない場合のみ範囲選択
        {
            if (!state->IsDragging() &&
                input_->IsMouseTriggered(0))
            {// ドラッグ開始
                state->StartDragSelection(mousePos);
            }

            else if (state->IsDragging() && input_->IsMousePressed(0))
            {// ドラッグ中
                state->UpdateDragSelection(mousePos);
            }
            else if (state->IsDragging() && input_->IsMouseReleased(0))
            {
                state->EndDragSelection();
                Vector2 LT, RB;
                state->GetDragSelectionRect(LT, RB);
                // 選択範囲内のノートを選択
                const auto& notes = document->GetData().notes;
                for (size_t i = 0; i < notes.size(); ++i)
                {
                    float noteX = coordinate->LaneToScreenX(notes[i].laneIndex);
                    float noteY = coordinate->TimeToScreenY(notes[i].targetTime);
                    if (noteX >= LT.x && noteX <= RB.x &&
                        noteY >= LT.y && noteY <= RB.y)
                    {
                        state->SelectNote(static_cast<uint32_t>(i), true); // マルチセレクトで追加
                    }
                }
            }
        }
    }
    // - ノートの移動
    {
        if (!state->IsMovingSelectedNote())
        {// 移動開始
            if (input_->IsMouseTriggered(0))
            {
                float mouseTime = coordinate->ScreenYToTime(mousePos.y);
                mouseTime = SnapTimeToGrid(mouseTime, state, document->GetData());
                state->StartMove(document->GetData().notes, mouseTime);
            }
        }
        else// 移動中
        {
            auto moveState = state->GetMoveState();
            float mouseTime =coordinate->ScreenYToTime(mousePos.y);
            float offset = SnapTimeToGrid(mouseTime, state, document->GetData());
            float deltaTime = offset - moveState.originalTimes.back();
            if (input_->IsMousePressed(0))
            {
                // ノート移動コマンド実行
                // TODO: コマンド修正
                auto command = std::make_unique<MoveNoteCommand>(
                    document,
                    moveState.movingIndices,
                    moveState.originalTimes,
                    deltaTime);
                // 移動中なので履歴には追加しない
                command->Execute();
            }
            // deltaTimeが0でコマンド実行している
            // からUndo Redoがうまく動かない
            //document->GetData().notes[lastNote].targetTime;が毎フレーム更新されているってこと
            // これを解決する必要がある
            else if (input_->IsMouseReleased(0))
            {
                // ノート移動コマンド実行
                auto command = std::make_unique<MoveNoteCommand>(
                    document,
                    moveState.movingIndices,
                    moveState.originalTimes,
                    deltaTime);
                commandHistory->ExecuteCommand(std::move(command));
                state->EndMove();
            }
        }
    }
}

// ========================================
// 通常ノート配置モード
// ========================================

void InputHandler::HandlePlaceNormalNoteMode(
    State* state,
    Document* document,
    CommandHistory* commandHistory,
    EditorCoordinate* coordinate)
{
    // TODO:OK 通常ノート配置の入力処理実装
    // - マウスクリックでノート配置
    // - グリッドスナップ適用

    if (input_->IsMouseTriggered(0)) // 左クリックでノート配置
    {
        int32_t laneIndex = GetLaneIndexFromMousePosition(coordinate);
        float noteTime = GetTimeFromMousePosition(coordinate);
        noteTime = SnapTimeToGrid(noteTime, state, document->GetData());

        if (laneIndex < 0 || laneIndex >= coordinate->GetLaneCount())
            return;

        auto command = std::make_unique<PlaceNoteCommand>(document, laneIndex, noteTime, "normal", 0.0f);
        commandHistory->ExecuteCommand(std::move(command));
    }

}

// ========================================
// ロングノート配置モード
// ========================================

void InputHandler::HandlePlaceLongNoteMode(
    State* state,
    Document* document,
    CommandHistory* commandHistory,
    EditorCoordinate* coordinate)
{
    // TODO:OK ロングノート配置の入力処理実装

    // Escキーでロングノート作成をキャンセル
    if (state->IsCreatingLongNote() && input_->IsKeyTriggered(DIK_ESCAPE))
    {
        state->EndLongNoteCreation();
        return;
    }
    // - ドラッグでロングノート作成
    // - 長さの調整

    const Vector2 mousePos = input_->GetMousePosition();
    if (input_->IsMouseTriggered(0))
    {
        float time = coordinate->ScreenYToTime(mousePos.y);
        time = SnapTimeToGrid(time, state, document->GetData());
        int32_t laneIndex = GetLaneIndexFromMousePosition(coordinate);

        state->StartLongNoteCreation(laneIndex, time);
    }
    else if (state->IsCreatingLongNote() && input_->IsMouseReleased(0))
    {
        float endTime = coordinate->ScreenYToTime(mousePos.y);
        endTime = SnapTimeToGrid(endTime, state, document->GetData());
        int32_t laneIndex = GetLaneIndexFromMousePosition(coordinate);
        float duration = endTime - state->GetLongNoteStartTime();

        if (duration > 0.0f)
        {
            auto command = std::make_unique<PlaceNoteCommand>(document, laneIndex, state->GetLongNoteStartTime(), "hold", duration);
            commandHistory->ExecuteCommand(std::move(command));
        }

        state->EndLongNoteCreation();
    }
}

// ========================================
// 削除モード
// ========================================

void InputHandler::HandleDeleteMode(
    Document* document,
    CommandHistory* commandHistory,
    EditorCoordinate* coordinate)
{
    // TODO:OK 削除モードの入力処理実装
    // - クリックでノート削除
    if (input_->IsMouseTriggered(0)) // 左クリックでノート削除
    {
        size_t noteIndex = GetNoteIndexFromMousePosition(document, coordinate);
        if (noteIndex != SIZE_MAX)
        {
            auto command = std::make_unique<DeleteNoteCommand>(document, noteIndex);
            commandHistory->ExecuteCommand(std::move(command));
        }
    }
}

void InputHandler::HandleBPMSettingMode(
    State* state,
    AudioController* audioController)
{
    state->GetTapBPMCounter().Update();
    audioController->PlayForBPMSet();
}

void InputHandler::HandleMouseWheelInput(EditorCoordinate* coordinate, AudioController* audioController, float& currentTime)
{
    if (!audioController)
        return;

    auto voice = audioController->GetVoiceInstance();
    if (voice && voice->IsPlaying())
        return;


    float wheelDelta = input_->GetMouseWheel();

    float left = coordinate->GetEditAreaX();
    float top = coordinate->GetTopMargin();
    float right = left + coordinate->GetEditAreaWidth();
    float bottom = top + coordinate->GetEditAreaHeight();

    Vector2 mousePos = input_->GetMousePosition();

    bool mouseInsideEditorArea =
        (mousePos.x >= left && mousePos.x <= right &&
         mousePos.y >= top && mousePos.y <= bottom);

    bool mouseInsideGameWindow =
        (mousePos.x >= 0 && mousePos.x <= WinApp::kWindowSize_.x &&
         mousePos.y >= 0 && mousePos.y <= WinApp::kWindowSize_.y);

    if (wheelDelta != 0.0f)
    {
        if (!mouseInsideEditorArea && mouseInsideGameWindow)
        {
            // スクロール量に応じて時間を更新
            float addedTime = wheelDelta * 0.1f / coordinate->GetZoom();
            // スクロール速度を調整
            // ゆっくりスクロールできるように。
            if (input_->IsKeyPressed(DIK_LSHIFT))
            {
                addedTime *= 0.1f;
            }
            currentTime += addedTime; // 現在の時間を更新
            currentTime = (std::max)(currentTime, 0.0f);
            coordinate->SetScrollOffset(currentTime); // スクロールオフセットを更新
        }
        else if (mouseInsideEditorArea)
        {
            float addedZoom = wheelDelta * 0.1f; // ホイールの動きに応じてズームを調整
            if (input_->IsKeyPressed(DIK_LSHIFT))
            {
                addedZoom *= 0.1f;
            }
            coordinate->SetZoom(coordinate->GetZoom() + addedZoom); // ズームを更新
        }
    }

    if (input_->IsMouseTriggered(2))
    {
        // スクリーン座標から時間に変換
        float targetTime = coordinate->ScreenYToTime(mousePos.y);
        auto musicSoundInstance = audioController->GetSoundInstance();
        if (musicSoundInstance)
        {
            currentTime = std::clamp(targetTime, 0.0f, musicSoundInstance->GetDuration()); // 音楽の再生時間を超えないように制限
        }
        else
        {
            currentTime = std::max(targetTime, 0.0f); // 負の時間を防ぐ
        }
    }

}

// ========================================
// ヘルパー関数
// ========================================

float InputHandler::SnapTimeToGrid(float time, const State* state, const BeatMapData& data) const
{
    if (!state->IsGridSnapEnabled())
        return time;

    float snapInterval = state->GetSnapInterval();
    float beatDuration = 60.0f / data.bpm;
    float gridInterval = beatDuration * snapInterval;

    return std::round(time / gridInterval) * gridInterval;
}

float InputHandler::GetTimeFromMousePosition(const EditorCoordinate* coordinate) const
{
    // TODO:OK 座標からの時間計算実装
    Vector2 mousePos = input_->GetMousePosition();
    return coordinate->ScreenYToTime(mousePos.y);
}

int32_t InputHandler::GetLaneIndexFromMousePosition(const EditorCoordinate* coordinate) const
{
    // TODO:OK 座標からのレーン計算実装
    Vector2 mousePos = input_->GetMousePosition();
    return coordinate->ScreenXToLane(mousePos.x);
}

size_t InputHandler::GetNoteIndexFromMousePosition(
    const Document* document,
    const EditorCoordinate* coordinate) const
{
    // 座標からのノート検索実装（ロングノート対応）

    auto notes = document->GetData().notes;
    const Vector2 mousePos = input_->GetMousePosition();

    for (size_t i = 0; i < notes.size(); ++i)
    {
        const auto& note = notes[i];
        if (!coordinate->IsNoteVisible(note.targetTime))
            continue;

        float noteX = coordinate->LaneToScreenX(note.laneIndex);
        const float laneWidth = coordinate->GetLaneWidth();

        // X座標の判定
        if (mousePos.x < noteX - laneWidth / 2.0f ||
            mousePos.x > noteX + laneWidth / 2.0f)
        {
            continue;
        }

        // Y座標の判定（通常ノートとロングノートで異なる）
        if (note.noteType == "long" && note.holdDuration > 0.0f)
        {
            // ロングノート: 開始位置から終了位置までの範囲で判定
            float startY = coordinate->TimeToScreenY(note.targetTime);
            float endY = coordinate->TimeToScreenY(note.targetTime + note.holdDuration);

            float topY = std::min(startY, endY);
            float bottomY = std::max(startY, endY);

            if (mousePos.y >= topY && mousePos.y <= bottomY)
            {
                return i; // ロングノートのインデックスを返す
            }
        }
        else
        {
            // 通常ノート: 開始位置のみで判定
            float noteY = coordinate->TimeToScreenY(note.targetTime);
            const float noteHeight = 25.0f; // ノートの高さ // TODO: 定数化

            if (mousePos.y >= noteY - noteHeight / 2.0f &&
                mousePos.y <= noteY + noteHeight / 2.0f)
            {
                return i; // 通常ノートのインデックスを返す
            }
        }
    }

    return SIZE_MAX;
}

} // namespace BME
