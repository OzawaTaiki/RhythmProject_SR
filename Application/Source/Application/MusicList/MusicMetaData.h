#pragma once

#include <string>

/// <summary>
/// 表示用の軽量データ
/// </summary>
struct MusicMetaData
{
    std::string filePath;
    std::string title;
    std::string artist;
    std::string audioFilePath;
    float bpm;
    uint32_t difficultyLevel;
    // noteデータは含めない
};