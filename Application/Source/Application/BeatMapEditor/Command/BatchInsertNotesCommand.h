#pragma once

#include "ICommand.h"
#include <Application/BeatMapLoader/BeatMapData.h>

namespace BME
{

class Document;

class BatchInsertNotesCommand : public ICommand
{
public:

    BatchInsertNotesCommand(Document* document, const std::vector<NoteData>& notes);

    void Execute() override;
    void Undo() override;

private:

    Document* document_ = nullptr; // BeatMapEditorのドキュメントへのポインタ

    std::vector<NoteData> notes_;
    std::vector<size_t> insertedIndices_; // 挿入されたノーツのインデックスを記録


};
}