#pragma once

#include <Application/BeatMapLoader/BeatMapData.h>
#include <Debug/Debug.h>

#include <string>
#include <future>
#include <vector>
#include <cstdint>

#include <json.hpp>

/// 譜面データローダークラス
class BeatMapLoader
{
public:
    // シングルトンインスタンス取得
    static BeatMapLoader* GetInstance();

    /// <summary>
    /// 譜面データを指定パスから非同期でロード
    /// </summary>
    /// <param name="beatMapPath">譜面データファイルパス</param>
    /// <returns>ロード成功ならtrue</returns>
    /// <remarks>ロード中はIsLoading()がtrueを返す</remarks>
    std::future<bool> LoadBeatMap(const std::string& beatMapPath);

    /// <summary>
    /// 譜面データを直接非同期でロード
    /// </summary>
    /// <param name="beatMapData">譜面データ</param>
    /// <returns>ロード成功ならtrue</returns>
    /// <remarks>ロード中はIsLoading()がtrueを返す</remarks>
    std::future<bool> LoadBeatMap(const BeatMapData& beatMapData);

    /// <summary>
    /// ロードした譜面データを取得
    /// </summary>
    /// <returns>ロードした譜面データ</returns>
    BeatMapData GetLoadedBeatMapData() const { return loadedBeatMapdata_; }

    /// <summary>
    /// ロード中かどうか取得
    /// </summary>
    /// <returns>ロード中ならtrue</returns>
    /// <remarks>非同期処理中に呼び出すことでロード状態を確認できる</remarks>
    bool IsLoading() const { return isLoading_; }

    /// <summary>
    /// ロード成功かどうか取得
    /// </summary>
    /// <returns>ロード成功ならtrue</returns>
    bool IsLoadingSuccess() const { return isLoadingSuccess_; }

    // エラーメッセージ取得
    std::string GetErrorMessage() const { return errorMessage_; }


private:
    template<typename T>
    bool GetIfExists(const std::string& key, const nlohmann::json& jsonData ,T* out);

    // JSONデータをBeatMapDataに変換
    BeatMapData ParseJsonToBeatMap(const nlohmann::json& jsonData);

private:

    bool isLoading_ = false; // ロード中フラグ
    std::string errorMessage_; // エラーメッセージ
    bool isLoadingSuccess_ = false; // ロード成功フラグ
    BeatMapData loadedBeatMapdata_; // ロードした譜面データ


private: // Singleton
    BeatMapLoader();
    ~BeatMapLoader();
    BeatMapLoader(const BeatMapLoader&) = delete;
    BeatMapLoader& operator=(const BeatMapLoader&) = delete;

};

template<typename T>
inline bool BeatMapLoader::GetIfExists(const std::string& key, const nlohmann::json& jsonData, T* out)
{
    if (!jsonData.contains(key))
        return false;

    try
    {
        jsonData.at(key).get_to(*out);
        return true;
    }
    catch (std::exception& e)
    {
        Debug::Log("BeatMapLoader::GetIfExists: Failed to get key " + key + " Exception: " + e.what());
        errorMessage_ = "Error: Key " + key + " has invalid type.";
        return false;
    }
}