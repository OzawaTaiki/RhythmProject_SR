#include "EditorState.h"
#include <Debug/Debug.h>
#include <algorithm>
#include <cmath>

using namespace Engine;

namespace BME
{
State::State()
{
    tapBPMCounter_.Initialize();
}
void State::SetCurrentMode(EditorMode mode)
{
    if (currentMode_ != mode)
    {
        previousMode_ = currentMode_;
        currentMode_ = mode;
    }
}

void State::SelectNote(uint32_t noteIndex, bool multiSelect)
{
    if (!multiSelect && !isRangeSelected_)
    {
        selectedNoteIndices_.clear();
    }

    auto it = std::find(
        selectedNoteIndices_.begin(),
        selectedNoteIndices_.end(),
        noteIndex);

    if (it == selectedNoteIndices_.end())
    {
        selectedNoteIndices_.push_back(noteIndex);
        // 二つ以上選択されている場合は範囲選択フラグを立てる
        if (selectedNoteIndices_.size() >= 2)
            isRangeSelected_ = true;
    }

    lastSelectedNoteIndex_ = noteIndex; // 最後に選択されたノートのインデックスを保存
    Debug::Log("Selected note at index " + std::to_string(noteIndex) + "\n");
}

void State::ClearSelection()
{
    selectedNoteIndices_.clear();
    isRangeSelected_ = false;
}

bool State::IsNoteSelected(uint32_t _noteIndex) const
{
    return std::find(selectedNoteIndices_.begin(), selectedNoteIndices_.end(), _noteIndex) != selectedNoteIndices_.end();
}


void State::Copy(const std::vector<NoteData>& notes, float baseline)
{
    if (selectedNoteIndices_.empty())
    {
        return;
    }

    clipboardData_.Clear(); // クリアしてからコピー

    clipboardData_.baseline = baseline; // 現在の時間をクリップボードのベースラインに設定

    for (size_t index : selectedNoteIndices_)
    {
        if (index < notes.size())
        {
            clipboardData_.notes.push_back(notes[index]); // 選択されたノートをクリップボードにコピー
        }
    }
}

std::vector<NoteData> State::GetPasteNotes(float currentTime) const
{
    std::vector<NoteData> pasteNotes;
    pasteNotes.reserve(clipboardData_.notes.size());

    // 時間オフセットを計算
    float timeOffset = currentTime - clipboardData_.baseline;

    for (const NoteData& note : clipboardData_.notes)
    {
        NoteData pasteNote = note;
        pasteNote.targetTime += timeOffset;
        pasteNotes.push_back(pasteNote);
    }

    return pasteNotes;
}

void State::ChangeEditorMode(EditorMode _mode)
{
    if (currentMode_ != _mode)
    {
        previousMode_ = currentMode_;
        currentMode_ = _mode;
    }

    if (currentMode_ == EditorMode::BPMSetting)
    {
        tapBPMCounter_.Reset();
    }
    if (previousMode_ == EditorMode::BPMSetting)
    {
        tapBPMCounter_.Reset();
    }
}

// ========================================
// ロングノート作成状態
// ========================================

void State::StartLongNoteCreation(uint32_t laneIndex, float startTime)
{
    isCreatingLongNote_ = true;
    longNoteLaneIndex_ = laneIndex;
    longNoteStartTime_ = startTime;
}

void State::EndLongNoteCreation()
{
    isCreatingLongNote_ = false;
    longNoteLaneIndex_ = 0;
    longNoteStartTime_ = 0.0f;
}

// =======================================
// ノート移動状態
// =======================================

void State::StartMove(const std::vector<NoteData>& notes,float mouseAtTime)
{
    if (selectedNoteIndices_.empty())
        return;

    moveState_.isMoving = true;
    moveState_.originalTimes.clear();
    moveState_.movingIndices.clear();
    moveState_.startMouseTime = mouseAtTime;


    for (size_t index : selectedNoteIndices_)
    {
        if (index < notes.size())
        {
            moveState_.originalTimes.push_back(notes[index].targetTime); // 元の時間を保存
            moveState_.movingIndices.push_back(index);
        }
    }
    moveState_.timeOffset = moveState_.originalTimes.back() - mouseAtTime;

}

void State::EndMove()
{
    moveState_.isMoving = false;
    moveState_.originalTimes.clear();
    moveState_.movingIndices.clear();
}

// ========================================
// ドラッグ選択状態
// ========================================

void State::StartDragSelection(const Vector2& start)
{
    isDragging_ = true;
    dragStartPosition_ = start;
    dragCurrentPosition_ = start;
}

void State::UpdateDragSelection(const Vector2& current)
{
    if (!isDragging_)
        return;

    dragCurrentPosition_ = current;
}

void State::EndDragSelection()
{
    isDragging_ = false;
}

void State::GetDragSelectionRect(Vector2& LT, Vector2& RB) const
{
    LT.x = std::min(dragStartPosition_.x, dragCurrentPosition_.x);
    LT.y = std::min(dragStartPosition_.y, dragCurrentPosition_.y);
    RB.x = std::max(dragStartPosition_.x, dragCurrentPosition_.x);
    RB.y = std::max(dragStartPosition_.y, dragCurrentPosition_.y);
}

} // namespace BME