#pragma once

#include <Features/Model/ObjectModel.h>
#include <vector>
#include <Features/Json/JsonBinder.h>
#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <System/Audio/SoundInstance.h>

namespace Engine { class Camera; }

class SpectrumFloor
{
public:

    void Initialize(int32_t row, int32_t col, const Engine::Vector2& arae);
    void Update(float deltaTime, Engine::AudioSpectrum* audioSpectrum, Engine::SoundInstance* soundinstance, float duration);
    void Draw(const Engine::Camera* camera);

private:


private:
    int32_t xCount_, zCount_;
    Engine::Vector3 basePos={};
    Engine::Vector2 areaSize_{};
    std::vector<std::vector<std::unique_ptr<Engine::ObjectModel>>> models_;

    struct TileData
    {
        float timer_ = 0.0f;
        bool isActive_ = false;

        Engine::Vector4 color_ = Engine::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    };
    // 縦のタイルとhz情報を持った構造体
    struct ColumnData
    {
        std::vector<TileData> tiles_;
        size_t beginIndex_ = 0;
        size_t endIndex_ = 0;
    };
    std::vector<ColumnData> columnsData_;

    std::vector<float> spectrumData_;
    float minHz_ = 200.0f;   //この値以下は切り捨てる
    float maxHz_ = 10000.0f;//この値以上は切り捨てる
    size_t beginIndex_ = 0;
    size_t endIndex_ = 0;

    std::unique_ptr<Engine::JsonBinder> jsonBinder_;
};
