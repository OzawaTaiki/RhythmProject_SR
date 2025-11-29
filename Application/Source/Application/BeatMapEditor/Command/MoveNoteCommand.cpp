#include "MoveNoteCommand.h"

#include <Application/BeatMapEditor/BeatMapDocument.h>

namespace BME
{

MoveNoteCommand::MoveNoteCommand(Document* document, const std::vector<size_t>& _noteIndices, const std::vector<float>& originalTimes, float _deltaTime) :
    document_(document),
    noteIndices_(_noteIndices),
    deltaTime_(_deltaTime)
{
    originalTimes_.reserve(noteIndices_.size());
    noteIdentifiers_.reserve(noteIndices_.size());
    // 元のノートの時間と識別子を保存
    for (size_t i = 0; i < _noteIndices.size(); ++i)
    {
        const NoteData& note = document->GetNoteAt(_noteIndices[i]);
        originalTimes_.push_back(originalTimes[i]);
        noteIdentifiers_.push_back({ originalTimes[i], note.laneIndex });
    }
}

void MoveNoteCommand::Execute()
{
    if (!document_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }

    // ノートの時間を更新
    for (uint32_t i = 0; i < noteIndices_.size(); ++i)
    {
        size_t index = noteIndices_[i];
        float newTime = originalTimes_[i] + deltaTime_;
        document_->SetNoteTime(index, newTime);
    }
}

void MoveNoteCommand::Undo()
{
    if (!document_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    // 元の時間に戻す
    for (uint32_t i = 0; i < noteIndices_.size(); ++i)
    {
        float currentTime = originalTimes_[i] + deltaTime_; // 移動後の時間

        int32_t noteIndex = document_->FindNoteAtTime(noteIdentifiers_[i].laneIndex, currentTime);
        if (noteIndex >= 0)
        {
            // ノートが見つかった場合、元の時間に戻す
            document_->SetNoteTime(static_cast<size_t>(noteIndex), originalTimes_[i]);
        }
    }
}

} // namespace BME
