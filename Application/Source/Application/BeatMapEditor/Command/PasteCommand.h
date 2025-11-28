#pragma once

#include <Application/BeatMapEditor/Command/ICommand.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>

namespace BME
{
class Document;

// コピーしたノートデータをペーストするコマンド
class PasteCommand : public ICommand
{
public:
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="_beatMapEditor">編集対象の BeatMapEditor ポインタ</param>
    /// <param name="_notesToPaste">コピーされたノートデータのリスト</param>
    /// <param name="_pasteOffset">ペースト時の時間オフセット（秒）</param>
    PasteCommand(Document* _beatMapEditor, const std::vector<NoteData>& _notesToPaste, float _pasteOffset = 0.0f);

    /// <summary>
    /// ノートをペーストして配置する処理を実行する。
    /// </summary>
    void Execute() override;

    /// <summary>
    /// ペーストしたノートを削除して元に戻す。
    /// </summary>
    void Undo() override;
private:

    Document* document_ = nullptr; // 譜面ドキュメントのポインタ
    std::vector<NoteData> notesToPaste_; // コピーしたノートデータ
    std::vector<size_t> pastedNoteIndices_; // ペーストしたノートのインデックス
    float pasteOffset_ = 0.0f; // ペーストオフセット時間
};
}// namespace BME
