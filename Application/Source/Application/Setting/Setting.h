#pragma once

#include <string>

struct GameSettings
{
    float masterVolume = 1.0f; // マスター音量
    float musicVolume = 1.0f; // 音楽音量
    float effectVolume = 1.0f; // 効果音音量

    float noteSpeed = 30.0f; // ノーツの移動速度
    float audioLatencyMs = 0.0f; // 音声遅延

};

class Setting
{
public:
    static GameSettings current_;
    static GameSettings default_;

    static void Load(const std::string& _filePath = "Resources/Data/Setting/setting.json");
    static void Save(const std::string& _filePath = "Resources/Data/Setting/setting.json");
};