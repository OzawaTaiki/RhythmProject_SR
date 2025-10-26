#pragma once

#include <Application/BeatMapEditor/Command/ICommand.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>

/// <summary>
/// ノート削除コマンド。
/// </summary>
class DeleteNoteCommand : public ICommand
{
public:

    /// <summary>
    /// 単一ノート削除用コンストラクタ。
    /// </summary>
    DeleteNoteCommand(BeatMapEditor* _beatMapEditor, uint32_t _noteIndex);

    /// <summary>
    /// 複数ノート削除用コンストラクタ。
    /// </summary>
    DeleteNoteCommand(BeatMapEditor* _beatMapEditor, std::vector<uint32_t> _noteIndex);

    /// <summary>
    /// ノートを削除する処理を実行する。
    /// </summary>
    void Execute() override;

    /// <summary>
    /// 削除したノートを元に戻す処理を行う。
    /// </summary>
    void Undo() override;


private:

    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ
    std::vector<uint32_t> noteIndex_; // 削除するノートのインデックス
    std::vector<NoteData> deletedNoteData_; // 削除したノートのデータを保存
};