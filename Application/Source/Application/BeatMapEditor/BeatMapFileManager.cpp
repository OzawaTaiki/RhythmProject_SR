#include "BeatMapFileManager.h"

#include <Application/BeatMapLoader/BeatMapLoader.h>
#include <Debug/Debug.h>
#include <Utility/StringUtils/StringUitls.h>

#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace BME {

FileManager::FileManager()
{
    beatMapLoader_ = BeatMapLoader::GetInstance();
}

// ========================================
// ロード処理
// ========================================

bool FileManager::Load(const std::string& _filePath, Document* _document)
{
    if (!beatMapLoader_ || !_document)
    {
        lastError_ = "BeatMapLoader or Document is null";
        return false;
    }

    auto future = beatMapLoader_->LoadBeatMap(_filePath);

    // ロードが完了するまで待機
    if (future.get())
    {
        BeatMapData loadedData = beatMapLoader_->GetLoadedBeatMapData();
        _document->SetData(loadedData);
        _document->SetModified(false);
        currentFilePath_ = _filePath;
        lastError_.clear();
        Engine::Debug::Log("Beatmap loaded successfully from: " + _filePath + "\n");
        return true;
    }
    else
    {
        lastError_ = beatMapLoader_->GetErrorMessage();
        Engine::Debug::Log("Error loading beatmap: " + lastError_ + "\n");
        return false;
    }
}

// ========================================
// 保存処理
// ========================================

bool FileManager::Save(Document* _document)
{
    if (currentFilePath_.empty())
    {
        lastError_ = "No file path set. Use SaveAs instead.";
        return false;
    }

    return SaveInternal(currentFilePath_, _document);
}

bool FileManager::SaveAs(const std::string& _filePath, Document* _document)
{
    return SaveInternal(_filePath, _document);
}

bool FileManager::SaveInternal(const std::string& _filePath, Document* _document)
{
    if (!_document)
    {
        lastError_ = "Document is null";
        return false;
    }

    std::string filePath = _filePath;
    std::string extension = StringUtils::GetExtension(filePath);
    if (extension != "json")
    {
        filePath += ".json"; // 拡張子がない場合は.jsonを追加
    }

    const BeatMapData& data = _document->GetData();

    // JSONデータ構築
    json j;
    j["title"] = data.title;
    j["artist"] = data.artist;
    j["audioFilePath"] = data.audioFilePath;
    j["bpm"] = data.bpm;
    j["offset"] = data.offset;
    j["difficultyLevel"] = data.difficultyLevel;

    // ノートデータをJSON配列に変換
    for (const auto& note : data.notes)
    {
        json noteJson;
        noteJson["laneIndex"] = note.laneIndex;
        noteJson["targetTime"] = note.targetTime;
        noteJson["noteType"] = note.noteType;
        noteJson["holdDuration"] = note.holdDuration;
        j["notes"].push_back(noteJson);
    }

    // ファイル書き出し
    std::ofstream outFile(filePath);
    if (!outFile.is_open())
    {
        lastError_ = "Failed to open file for saving: " + filePath;
        Engine::Debug::Log(lastError_ + "\n");
        return false;
    }

    outFile << j.dump(4); // JSONを整形して書き出し
    outFile.close();

    // 保存成功時の処理
    currentFilePath_ = filePath;
    _document->SetModified(false);
    lastError_.clear();
    Engine::Debug::Log("Beatmap saved successfully to: " + filePath + "\n");

    return true;
}

// ========================================
// 新規作成
// ========================================

void FileManager::CreateNew(const std::string& _filePath, const std::string& _audioFilePath, Document* _document)
{
    if (!_document)
    {
        lastError_ = "Document is null";
        return;
    }

    // 新規BeatMapDataを作成
    BeatMapData newData;
    newData.title = "None";
    newData.artist = "unknown";
    newData.audioFilePath = _audioFilePath;
    newData.bpm = 120.0f;
    newData.offset = 0.0f;
    newData.difficultyLevel = 3;
    newData.notes.clear();

    _document->SetData(newData);
    _document->SetModified(false);
    currentFilePath_ = _filePath;
    lastError_.clear();

    Debug::Log("New beatmap created\n");
}

} // namespace BME
