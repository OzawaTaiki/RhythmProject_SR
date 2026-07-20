#pragma once

#include <memory>

#include <Features/Scene/SceneData.h>
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/Result/ResultData.h>
#include <Application/SpectrumRing/SpectrumRing.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UVTransform/UVTransformAnimation.h>
#include <Application/TitleCamera/TitleCamera.h>
#include <Application/HexagonGrid/HexagonGrid.h>

// シーン間でデータを受け渡すための構造体群
/// <summary>
/// シーン間で受け渡すデータ構造体群を定義するファイル。
/// </summary>
// セレクトシーンからゲームシーンへ渡すデータ
/// <summary>セレクトシーン → ゲームシーンへ渡すデータ。選択された譜面ファイルパスを保持する。</summary>
struct SelectToGameData : Engine::SceneData
{
    std::string selectedBeatMapFilePath; // 選択された譜面ファイルのパス
};
// 共有される譜面データ
/// <summary>複数シーン間で共有する譜面データ。ロード済み BeatMapData をそのまま引き継ぐ。</summary>
struct SharedBeatMapData : Engine::SceneData
{
    BeatMapData beatMapData; // 前のシーンから渡される譜面データ
};
// ゲームシーンからリザルトシーンへ渡すデータ
/// <summary>ゲームシーン → リザルトシーンへ渡すデータ。スコアや判定結果を保持する ResultData をラップする。</summary>
struct GameToResultData : Engine::SceneData
{
    ResultData resultData; // ゲームの結果データ
};

/// <summary>タイトルシーン → セレクトシーンへ渡すデータ。ロビー演出に必要なリソースを引き継ぎ、再初期化コストを省く。</summary>
struct TitleToSelectData : Engine::SceneData
{
    std::shared_ptr<SpectrumRing> spectrumRing; // タイトルシーンから引き継がれるスペクトラムリング
    std::shared_ptr<Engine::VoiceInstance> voiceInstance; // タイトルシーンから引き継がれる音楽インスタンス
    std::unique_ptr<LobbyCamera> lobbyCamera; // タイトルシーンから引き継がれるロビー用カメラ
    std::shared_ptr<Engine::UIImageElement> titleBackground; // タイトルシーンから引き継がれるタイトル背景のUI要素
    Engine::UVTransformAnimation titleBackgroundAnimation; // タイトル背景のUV変換アニメーション
    std::shared_ptr<HexagonGrid> hexagonGrid; // タイトルシーンから引き継がれるヘキサゴングリッド
};