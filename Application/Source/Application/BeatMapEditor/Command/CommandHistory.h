#pragma once

#include "ICommand.h"

#include <deque>
#include <memory>

/// <summary>
/// コマンド履歴管理クラス
/// </summary>
/// <remarks>
/// コマンドの実行、元に戻す、やり直すの履歴を管理する
/// </remarks>
class CommandHistory
{
public:

    void ExecuteCommand(std::unique_ptr<ICommand> command);

    bool CanUndo() const;
    bool CanRedo() const;

    void Undo();
    void Redo();

    // 履歴をクリア
    void Clear();

    // 最大のundo数制限を設定
    void SetMaxUndoSize(int32_t size) { maxUndoSize_ = size; }
    int32_t GetMaxUndoSize() const { return maxUndoSize_; }
private:

    std::deque<std::unique_ptr<ICommand>> undoStack_;  // 元に戻すコマンドのスタック
    std::deque<std::unique_ptr<ICommand>> redoStack_;  // やり直すコマンドのスタック

    int32_t maxUndoSize_ = 50; // 最大のundo数制限（必要に応じて調整可能）
};