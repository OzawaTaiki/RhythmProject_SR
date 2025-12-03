#pragma once

#include <Application/BeatMapLoader/BeatMapData.h>

#include <string>

namespace BME // Beat Map Editor
{
/// <summary>
/// 譜面データ全体の管理クラス
/// </summary>
/// <remarks>
/// コマンド生成時に渡して譜面データの変更を行う
/// </remarks>
class Document
{
public:
    void PlaceNote(const NoteData& note);
    NoteData DeleteNote(size_t noteIndex);
    size_t InsertNote(const NoteData& note);

    // データアクセス
    const BeatMapData& GetData() const { return data_; }
    BeatMapData& GetMutableData() { return data_; }
    void SetData(const BeatMapData& data) { data_ = data; }

    bool IsModified() const { return isModified_; }
    void SetModified(bool modified) { isModified_ = modified; }


    /// <summary>
    /// ノートの時間を変更
    /// </summary>
    void SetNoteTime(size_t noteIndex, float newTime);

    /// <summary>
    /// 指定した時間とレーンのノートを検索
    /// </summary>
    int32_t FindNoteAtTime(int32_t laneIndex, float time) const;

    /// <summary>
    /// ノートを取得
    /// </summary>
    const NoteData& GetNoteAt(size_t index) const;


private:
    /// <summary>
    /// ノートを時間でソート
    /// </summary>
    void SortNotesByTime();

    /// <summary>
    /// ノートを挿入する位置を検索
    /// </summary>
    size_t FindInsertIndex(const NoteData& note) const;

private:
    BeatMapData data_;    // 譜面データ
    bool isModified_ = false;           // 譜面が変更されたかどうかのフラグ

};


}// namespace BME