#include "PlaceNoteCommand.h"

#include <Application/BeatMapEditor/BeatMapDocument.h>

namespace BME
{
PlaceNoteCommand::PlaceNoteCommand(Document* document, uint32_t laneIndex, float targetTime, const std::string& noteType, float holdDuration):
    document_(document),
    laneIndex_(laneIndex),
    targetTime_(targetTime),
    noteType_(noteType),
    holdDuration_(holdDuration),
    placeNoteIndex_(SIZE_MAX)
{
}

void PlaceNoteCommand::Execute()
{
    if (!document_)
        return;

    NoteData note;
    note.laneIndex = laneIndex_;
    note.targetTime = targetTime_;
    note.noteType = noteType_;
    note.holdDuration = holdDuration_;


    // ノートを配置
    placeNoteIndex_ = document_->InsertNote(note);
}

void PlaceNoteCommand::Undo()
{
    if (!document_)
        return;

    // ノートを削除
    document_->DeleteNote(static_cast<uint32_t>(placeNoteIndex_));
}

} // namespace BME