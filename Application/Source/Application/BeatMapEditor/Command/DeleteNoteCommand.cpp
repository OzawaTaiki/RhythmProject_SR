#include "DeleteNoteCommand.h"

#include <Application/BeatMapEditor/BeatMapDocument.h>

namespace BME
{

DeleteNoteCommand::DeleteNoteCommand(Document* document, size_t noteIndex) :
    document_(document),
    noteIndex_({ noteIndex })
{
}

DeleteNoteCommand::DeleteNoteCommand(Document* document, const std::vector<size_t>& noteIndex) :
    document_(document),
    noteIndex_(noteIndex)
{
}

void DeleteNoteCommand::Execute()
{
    if (!document_)
    {
        return;
    }

    // 以前の削除データをクリア
    deletedNoteData_.clear();

    for (size_t index : noteIndex_)
    {
        deletedNoteData_.push_back(document_->DeleteNote(index));
    }
}

void DeleteNoteCommand::Undo()
{
    if (!document_)
    {
        return;
    }

    for (const auto& note : deletedNoteData_)
    {
        // 削除したノートを元に戻す
        document_->InsertNote(note);
    }
}

} // namespace BME