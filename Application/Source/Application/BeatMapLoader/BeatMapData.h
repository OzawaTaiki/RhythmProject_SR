#pragma once

#include <string>
#include <vector>
#include <cstdint>

// ノートデータ構造体
struct NoteData
{
    uint32_t laneIndex; // 流れるレーンインデックス
    float targetTime; // タイミング
    std::string noteType; // ノートの種類 (normal, long)
    float holdDuration; // ホールドノートの持続時間

    NoteData() : laneIndex(0), targetTime(0.0f), noteType("normal"), holdDuration(0.0f) {}
    NoteData(uint32_t _laneIndex, float _targetTime, const std::string& _noteType, float _holdDuration)
        : laneIndex(_laneIndex), targetTime(_targetTime), noteType(_noteType), holdDuration(_holdDuration){}
};

// 譜面データ構造体
/// <summary>
/// 譜面（BeatMap）データを表す構造体。譜面のメタ情報とノート列を保持する。
/// </summary>
struct BeatMapData
{
    std::string title = "None"; // 曲のタイトル
    std::string artist = "unknown"; // アーティスト名 仮
    std::string audioFilePath ="none"; // 音声ファイルのパス
    float bpm = 120.0f; // tempo
    float offset=0.0f; // オフセット時間

    uint32_t difficultyLevel = 3; // 難易度レベル : 仮

    std::vector<NoteData> notes = {}; // ノートデータのリスト

    BeatMapData() : bpm(0.0f), offset(0.0f), difficultyLevel(0) {}

};