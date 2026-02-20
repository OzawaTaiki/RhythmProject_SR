#include "MusicListManager.h"
#include <Features/Json/Loader/JsonFileIO.h>
#include <Features/Json/JsonUtils.h>

#include <filesystem>

using namespace Engine;

MusicListManager* MusicListManager::GetInstance()
{
    static MusicListManager instance;
    return &instance;
}

void MusicListManager::LoadAync()
{
    isLoadComplete_ = false;

    loadFuture_ = std::async(std::launch::async, [this]()
                             {
                                 ScanFolder();
                                 isLoadComplete_ = true;
                             });

}

bool MusicListManager::IsLoadComplete() const
{
    return isLoadComplete_;
}

size_t MusicListManager::GetMusicCount() const
{
    return musicList_.size();
}

const std::vector<MusicMetaData>& MusicListManager::GetMusicList() const
{
    return musicList_;
}

MusicMetaData MusicListManager::GetMusicMetaDataAt(size_t index) const
{
    if (index >= 0 && musicList_.size() > index)
        return musicList_.at(index);

    return MusicMetaData();
}

void MusicListManager::Refresh()
{
    LoadAync();
}

void MusicListManager::ScanFolder()
{
    musicList_.clear();

    // 繝・ぅ繝ｬ繧ｯ繝医Μ縺悟ｭ伜惠縺励↑縺代ｌ縺ｰ邨ゆｺ・
    if (!std::filesystem::exists(musicDirectory_))
        return;

    for (const auto& entry : std::filesystem::directory_iterator(musicDirectory_))
    {
        // 繝輔ぃ繧､繝ｫ縺ｧ縺ｪ縺代ｌ縺ｰ繧ｹ繧ｭ繝・・
        if (!entry.is_regular_file())
            continue;

        const auto& path = entry.path();
        if (path.extension() != ".json") // json莉･螟悶・繧ｹ繧ｭ繝・・
            continue;

        MusicMetaData metaData = LoadMusicMetaData(path.filename().string());
        if (metaData.title.empty()|| metaData.audioFilePath.empty())
            continue;

        musicList_.push_back(metaData);
    }

}

MusicMetaData MusicListManager::LoadMusicMetaData(const std::string& filePath) const
{
    MusicMetaData metaData;

    auto jsonData = JsonFileIO::Load(filePath, musicDirectory_);

    metaData.title              = JsonUtils::GetOrDefault<std::string>("title", jsonData, "");
    metaData.artist             = JsonUtils::GetOrDefault<std::string>("artist", jsonData, "");
    metaData.audioFilePath      = JsonUtils::GetOrDefault<std::string>("audioFilePath", jsonData, "");
    metaData.bpm                = JsonUtils::GetOrDefault<float>("bpm", jsonData, 120.0f);
    metaData.difficultyLevel    = JsonUtils::GetOrDefault<uint32_t>("difficultyLevel", jsonData, 1);
    metaData.filePath           = musicDirectory_ + filePath;
    return metaData;
}
