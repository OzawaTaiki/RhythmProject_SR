#pragma once

#include "MusicMetaData.h"

#include <vector>
#include <future>

/// <summary>
/// 音楽リスト管理クラス
/// </summary>
class MusicListManager
{
public:
    static MusicListManager* GetInstance();

    /// <summary>
    /// 非同期で音楽リストを読み込み
    /// </summary>
    void LoadAync();

    /// <summary>
    /// 読み込み完了しているか
    /// </summary>
    /// <returns>完了していればtrue</returns>
    bool IsLoadComplete() const;

    /// <summary>
    /// 音楽数を取得
    /// </summary>
    /// <returns>音楽数</returns>
    size_t GetMusicCount() const;

    /// <summary>
    /// 音楽リストを取得
    /// </summary>
    const std::vector<MusicMetaData>& GetMusicList() const;

    /// <summary>
    /// 音楽リストを再読み込み
    /// </summary>
    void Refresh();

private:

    /// <summary>
    /// フォルダ内の走査処理(別スレッド)
    /// </summary>
    void ScanFolder();

    /// <summary>
    /// 音楽メタデータの読み込み
    /// </summary>
    MusicMetaData LoadMusicMetaData(const std::string& filePath) const;


private:

    std::vector<MusicMetaData> musicList_;
    std::string musicDirectory_ = "Resources/Data/Game/BeatMap/";

    std::future<void> loadFuture_;

    bool isLoadComplete_ = false;

private:
    MusicListManager() = default;
    ~MusicListManager() = default;
    MusicListManager(const MusicListManager&) = delete;
    MusicListManager& operator=(const MusicListManager&) = delete;
};
