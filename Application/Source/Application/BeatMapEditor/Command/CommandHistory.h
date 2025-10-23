#pragma once

#include "ICommand.h"

#include <deque>
#include <memory>

/// <summary>
/// コマンド履歴管理クラス。
/// </summary>
/// <remarks>
/// コマンドの実行、元に戻す (Undo)、やり直す (Redo) の履歴を管理します。
/// </remarks>
class CommandHistory
{
public:
    /// <summary>
    /// コマンドを実行して履歴に追加する。
    /// </summary>
    /// <param name="command">実行するコマンド（unique_ptrで受け取る）</param>
    void ExecuteCommand(std::unique_ptr<ICommand> command);

    /// <summary>
    /// Undoが可能かどうかを返す。
    /// </summary>
    bool CanUndo() const;

    /// <summary>
    /// Redoが可能かどうかを返す。
    /// </summary>
    bool CanRedo() const;

    /// <summary>
    /// 履歴の最後のコマンドを元に戻す。
    /// </summary>
    void Undo();

    /// <summary>
    /// 直近で元に戻したコマンドを再実行する。
    /// </summary>
    void Redo();

    /// <summary>
    /// 履歴をクリアする。
    /// </summary>
    void Clear();

    /// <summary>
    /// 最大のundo数制限を設定する。
    /// </summary>
    void SetMaxUndoSize(int32_t size) { maxUndoSize_ = size; }

    /// <summary>
    /// 最大のundo数制限を取得する。
    /// </summary>
    int32_t GetMaxUndoSize() const { return maxUndoSize_; }
private:

    std::deque<std::unique_ptr<ICommand>> undoStack_;  // 元に戻すコマンドのスタック
    std::deque<std::unique_ptr<ICommand>> redoStack_;  // やり直すコマンドのスタック

    int32_t maxUndoSize_ = 50; // 最大のundo数制限（必要に応じて調整可能）
};