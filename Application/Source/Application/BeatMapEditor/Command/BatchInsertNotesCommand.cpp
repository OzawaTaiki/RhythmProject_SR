#include "BatchInsertNotesCommand.h"

#include "../BeatMapDocument.h"

namespace BME
{

BatchInsertNotesCommand::BatchInsertNotesCommand(Document* document, const std::vector<NoteData>& notes) :
    document_(document),
    notes_(notes)
{
}

void BatchInsertNotesCommand::Execute()
{
    if (!document_)
        return;

    for (const auto& note : notes_)
    {
        size_t index = document_->InsertNote(note);
        insertedIndices_.push_back(index);
    }
}

void BatchInsertNotesCommand::Undo()
{
    if (!document_)
        return;

    // 挿入されたノーツを逆順で削除していく
    // 逆順で削除するのは、インデックスが後ろのノーツから削除していかないと、
    // 前のノーツを削除したときに後ろのノーツのインデックスが変わってしまうため
    for (auto it = insertedIndices_.rbegin(); it != insertedIndices_.rend(); ++it)
    {
        document_->DeleteNote(*it);
    }
}

}// namespace BME
