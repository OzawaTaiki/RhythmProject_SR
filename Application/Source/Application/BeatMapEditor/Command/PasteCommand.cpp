#include "PasteCommand.h"

#include <Application/BeatMapEditor/BeatMapDocument.h>

namespace BME
{

PasteCommand::PasteCommand(Document* document, const std::vector<NoteData>& _notesToPaste, float _pasteOffset):
    document_(document),
    notesToPaste_(_notesToPaste),
    pasteOffset_(_pasteOffset)
{
}

void PasteCommand::Execute()
{
    if (!document_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    if (notesToPaste_.empty())
    {
        return; // コピーしたノートがない場合は何もしない
    }
    // ペーストオフセットを考慮してノートを配置
    for (const auto& note : notesToPaste_)
    {
        NoteData newNote = note;
        newNote.targetTime += pasteOffset_; // オフセットを適用
        size_t noteIndex = document_->InsertNote(newNote); // ノートを挿入
        if (noteIndex != SIZE_MAX) // ノートが正常に配置された場合
        {
            pastedNoteIndices_.push_back(noteIndex); // 配置したノートのインデックスを保存
        }
    }
}

void PasteCommand::Undo()
{
    if (!document_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    // ペーストしたノートを削除
    for (int32_t i = 0; i < pastedNoteIndices_.size(); ++i)
    {
        size_t index = pastedNoteIndices_[i];
        document_->DeleteNote(index - i); // 消された分詰められているのでその分を考慮する
    }
    pastedNoteIndices_.clear(); // 削除後はインデックスをクリア
}

} // namespace BME