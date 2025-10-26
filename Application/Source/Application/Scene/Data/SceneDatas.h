#pragma once

#include <Features/Scene/SceneData.h>
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/Result/ResultData.h>

// シーン間でデータを受け渡すための構造体群
/// <summary>
/// シーン間で受け渡すデータ構造体群を定義するファイル。
/// </summary>
// セレクトシーンからゲームシーンへ渡すデータ
struct SelectToGameData : SceneData
{
    std::string selectedBeatMapFilePath; // 選択された譜面ファイルのパス
};
// 共有される譜面データ
struct SharedBeatMapData : SceneData
{
    BeatMapData beatMapData; // 前のシーンから渡される譜面データ
};
// ゲームシーンからリザルトシーンへ渡すデータ
struct GameToResultData : SceneData
{
    ResultData resultData; // ゲームの結果データ
};