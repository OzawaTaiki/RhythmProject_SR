#include "Setting.h"

#include <Features/Json/Loader/JsonFileIO.h>

GameSettings Setting::default_ = {
    1.0f, // masterVolume
    1.0f, // musicVolume
    1.0f, // effectVolume
    30.0f, // noteSpeed
    0.0f // audioLatencyMs
};
GameSettings Setting::current_ = Setting::default_;

void Setting::Load(const std::string& _filePath)
{
    json data = JsonFileIO::Load(_filePath, "");
    if (data.is_null())
    {
        current_ = default_;
        return;
    }

    if (data.contains("masterVolume"))          current_.masterVolume   = data["masterVolume"].get<float>();
    if (data.contains("musicVolume"))           current_.musicVolume    = data["musicVolume"].get<float>();
    if (data.contains("effectVolume"))          current_.effectVolume   = data["effectVolume"].get<float>();
    if (data.contains("noteSpeed"))             current_.noteSpeed      = data["noteSpeed"].get<float>();
    if (data.contains("audioLatencyMs"))        current_.audioLatencyMs = data["audioLatencyMs"].get<float>();

}

void Setting::Save(const std::string& _filePath)
{
    json data;

    data["masterVolume"] = current_.masterVolume;
    data["musicVolume"] = current_.musicVolume;
    data["effectVolume"] = current_.effectVolume;
    data["noteSpeed"] = current_.noteSpeed;
    data["audioLatencyMs"] = current_.audioLatencyMs;

    JsonFileIO::Save(_filePath, "", data);
}