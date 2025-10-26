#pragma once

#include <Application/BeatMapEditor/Command/ICommand.h>

#include <string>
#include <cstdint>

class BeatMapEditor;

// ノート配置コマンドクラス
class PlaceNoteCommand : public ICommand
{
public:
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="_beatMapEditor">編集対象の BeatMapEditor ポインタ</param>
    /// <param name="_laneIndex">配置するレーンのインデックス</param>
    /// <param name="_targetTime">ノートのターゲット時間（秒）</param>
    /// <param name="_noteType">ノート種別（"normal" や "long" など）</param>
    /// <param name="_holdDuration">ホールドノートの持続時間（秒）</param>
    PlaceNoteCommand(BeatMapEditor* _beatMapEditor, uint32_t _laneIndex, float _targetTime, const std::string& _noteType, float _holdDuration = 0.0f);

    /// <summary>
    /// ノートを配置する処理を実行する。
    /// </summary>
    void Execute() override;

    /// <summary>
    /// 配置したノートを削除して元に戻す処理。
    /// </summary>
    void Undo() override;

private:
    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ

    // 配置するノートのデータ
    uint32_t laneIndex_ = 0; // レーンインデックス
    float targetTime_ = 0.0f; // ターゲット時間
    std::string noteType_ = "normal"; // ノートの種類 (normal, long)
    float holdDuration_ = 0.0f; // ホールドノートの持続時間

    size_t placeNoteIndex_ = 0; // 配置したノートのインデックス


};