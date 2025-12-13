#include "BeatMapLoader.h"

#include <json.hpp>
#include <fstream>

BeatMapLoader::BeatMapLoader()
{
}

BeatMapLoader::~BeatMapLoader()
{
}

BeatMapLoader* BeatMapLoader::GetInstance()
{
    static BeatMapLoader instance;
    return &instance;
}

std::future<bool> BeatMapLoader::LoadBeatMap(const std::string& filePath)
{
    // すでにロード中なら拒否
    if (isLoading_) {
        std::promise<bool> promise;
        promise.set_value(false);
        return promise.get_future();
    }

    // ロード状態初期化
    isLoading_ = true;
    isLoadingSuccess_ = false;
    errorMessage_.clear();

    // 非同期処理を開始
    return std::async(std::launch::async, [this, filePath]() {
        try {
            // ファイル読み込み
            std::ifstream file(filePath);
            if (!file.is_open()) {
                errorMessage_ = "Could not open file: " + filePath;
                isLoading_ = false;
                return false;
            }

            // JSONパース
            nlohmann::json jsonData;
            file >> jsonData;
            file.close();

            // 譜面データ変換
            loadedBeatMapdata_ = ParseJsonToBeatMap(jsonData);

            // 成功フラグ設定
            isLoadingSuccess_ = true;

        }
        catch (const std::exception& e) {
            // エラー発生時
            errorMessage_ = e.what();
            isLoadingSuccess_ = false;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2)); // 2s待機
        // ロード完了
        isLoading_ = false;

        return isLoadingSuccess_;
        });
}

std::future<bool> BeatMapLoader::LoadBeatMap(const BeatMapData& beatMapData)
{
    // すでにロード中なら拒否
    if (isLoading_) {
        std::promise<bool> promise;
        promise.set_value(false);
        return promise.get_future();
    }
    // ロード状態初期化
    isLoading_ = true;
    isLoadingSuccess_ = false;
    errorMessage_.clear();
    // 非同期処理を開始
    return std::async(std::launch::async, [this, beatMapData]() {
        try {
            // 譜面データを直接設定
            loadedBeatMapdata_ = beatMapData;
            // 成功フラグ設定
            isLoadingSuccess_ = true;
        }
        catch (const std::exception& e) {
            // エラー発生時
            errorMessage_ = e.what();
            isLoadingSuccess_ = false;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(500)); // .5s待機
        // ロード完了
        isLoading_ = false;
        return isLoadingSuccess_;
        });
}


BeatMapData BeatMapLoader::ParseJsonToBeatMap(const nlohmann::json& jsonData)
{
    BeatMapData data;

    // 曲のタイトル
    GetIfExists("title", jsonData, &data.title);
    GetIfExists("artist", jsonData, &data.artist);
    GetIfExists("audioFilePath", jsonData, &data.audioFilePath);
    GetIfExists("bpm", jsonData, &data.bpm);
    GetIfExists("offset", jsonData, &data.offset);
    GetIfExists("difficultyLevel", jsonData, &data.difficultyLevel);

    // ノートデータ
    if (jsonData.contains("notes") && jsonData["notes"].is_array())
    {
        for (const auto& note : jsonData["notes"])
        {
            NoteData noteData;
            GetIfExists("laneIndex", note, &noteData.laneIndex);
            GetIfExists("targetTime", note, &noteData.targetTime);
            GetIfExists("noteType", note, &noteData.noteType);
            GetIfExists("holdDuration", note, &noteData.holdDuration);
            data.notes.push_back(noteData);
        }

        // dataをtargetTImeの昇順でソート
        std::sort(data.notes.begin(), data.notes.end(), [](const NoteData& a, const NoteData& b)
                  {
                      return a.targetTime < b.targetTime;
                  });
    }

    return data;
}
