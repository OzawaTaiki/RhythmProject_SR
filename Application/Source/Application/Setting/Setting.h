#pragma once

#include <string>

/// <summary>
/// ゲーム設定データ構造体。
/// </summary>
struct GameSettings
{
    float masterVolume = 1.0f; // マスター音量
    float musicVolume = 1.0f; // 音楽音量
    float effectVolume = 1.0f; // 効果音音量

    float noteSpeed = 30.0f; // ノーツの移動速度
    float audioLatencyMs = 0.0f; // 音声遅延

};

/// <summary>
/// 設定管理クラス。
/// </summary>
class Setting
{
public:
    static GameSettings current_; // 現在の設定
    static GameSettings default_; // デフォルト設定

    /// <summary>
    /// 設定をファイルから読み込む。
    /// </summary>
    /// <param name="filePath">読み込むファイルパス（デフォルト値あり）</param>
    static void Load(const std::string& filePath = "Resources/Data/Setting/setting.json");

    /// <summary>
    /// 設定をファイルへ保存する。
    /// </summary>
    /// <param name="filePath">保存先ファイルパス（デフォルト値あり）</param>
    static void Save(const std::string& filePath = "Resources/Data/Setting/setting.json");
};