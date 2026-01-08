#pragma once

#include <Features/Model/ObjectModel.h>
#include <vector>
#include <Features/Json/JsonBinder.h>
#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <System/Audio/SoundInstance.h>

class Camera;

class SpectrumFloor
{
public:

    void Initialize(int32_t row, int32_t col, const Vector2& arae);
    void Update(float deltaTime, AudioSpectrum* audioSpectrum,SoundInstance* soundinstance,float duration);
    void Draw(const Camera* camera);

private:


private:
    int32_t xCount_, zCount_;
    Vector3 basePos={};
    Vector2 areaSize_{};
    std::vector<std::vector<std::unique_ptr<ObjectModel>>> models_;

    struct TileData
    {
        float timer_ = 0.0f;
        bool isActive_ = false;

        Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
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

    std::unique_ptr<JsonBinder> jsonBinder_;
};
