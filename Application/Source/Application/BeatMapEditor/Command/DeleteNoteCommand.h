#pragma once

#include <Application/BeatMapEditor/Command/ICommand.h>

#include <Application/BeatMapLoader/BeatMapData.h>

#include <cstdint>
#include <vector>

namespace BME
{

class State;
class Document;

/// <summary>
/// ノート削除コマンド。
/// </summary>
class DeleteNoteCommand : public ICommand
{
public:

    /// <summary>
    /// 単一ノート削除用コンストラクタ。
    /// </summary>
    DeleteNoteCommand( Document* document, size_t noteIndex);

    /// <summary>
    /// 複数ノート削除用コンストラクタ。
    /// </summary>
    DeleteNoteCommand( Document* document, const std::vector<size_t>& noteIndex);

    /// <summary>
    /// ノートを削除する処理を実行する。
    /// </summary>
    void Execute() override;

    /// <summary>
    /// 削除したノートを元に戻す処理を行う。
    /// </summary>
    void Undo() override;


private:

    Document* document_ = nullptr; // 譜面ドキュメントのポインタ
    std::vector<size_t> noteIndex_; // 削除するノートのインデックスリスト
    std::vector<NoteData> deletedNoteData_; // 削除したノートのデータを保存
};

} // namespace BME