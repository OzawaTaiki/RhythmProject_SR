#pragma once

#include <Application/BeatMapEditor/Command/ICommand.h>


#include <vector>
#include <cstdint>

namespace BME
{
class Document;

// ノートを移動するコマンド
class MoveNoteCommand : public ICommand
{
public:
    /// <summary>
    /// 指定されたノートを指定時間分移動するコマンド
    /// </summary>
    /// <param name="_editor">BeatMapEditorのポインタ</param>
    /// <param name="_noteIndices">移動対象ノートのインデックスリスト</param>
    /// <param name="_deltaTime">移動量（秒）正数で未来方向</param>
    MoveNoteCommand(Document* doxcument, const std::vector<size_t>& noteIndices, const std::vector<float>& originalTimes,float deltaTime);

    void Execute() override;
    void Undo() override;
private:
    Document* document_ = nullptr; // 譜面ドキュメントのポインタ


    std::vector<size_t> noteIndices_; // 移動するノートのインデックス
    std::vector<float> originalTimes_; // 元のノートの時間

    float deltaTime_ = 0.0f; // 移動する時間の差分

    struct NoteIdentifier
    {
        float time; // ノートの時間
        int32_t laneIndex; // レーンインデックス
    };

    std::vector<NoteIdentifier> noteIdentifiers_; // ノートの識別子リスト

};
} // namespace BME