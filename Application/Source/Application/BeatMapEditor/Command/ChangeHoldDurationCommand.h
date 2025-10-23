#pragma once

#include <Application/BeatMapEditor/Command/ICommand.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>

/// <summary>
/// ノートの持続時間を変更するコマンド。
/// </summary>
class ChangeHoldDurationCommand : public ICommand
{
public:
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="_beatMapEditor">編集対象の BeatMapEditor ポインタ</param>
    /// <param name="_noteIndex">対象ノートのインデックス</param>
    /// <param name="_oldHoldDuration">変更前の持続時間</param>
    /// <param name="_newHoldDuration">変更後の持続時間</param>
    ChangeHoldDurationCommand(BeatMapEditor* _beatMapEditor, uint32_t _noteIndex, float _oldHoldDuration, float _newHoldDuration);

    /// <summary>
    /// コマンドを実行してノートの持続時間を新しい値に変更する。
    /// </summary>
    void Execute() override;

    /// <summary>
    /// コマンドを元に戻してノートの持続時間を以前の値に戻す。
    /// </summary>
    void Undo() override;

private:
    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ
    uint32_t noteIndex_ = 0; // 持続時間を変更するノートのインデックス
    float newHoldDuration_ = 0.0f; // 新しい持続時間
    float oldHoldDuration_ = 0.0f; // 元の持続時間を保存
};