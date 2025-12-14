#pragma once
#include <array>


class ComboThresholds
{
public:
    ComboThresholds() = default;
    ~ComboThresholds() = default;

    void Initialize(int32_t maxCombo);

    int32_t GetComboLevel(int32_t combo) const;

    float GetComboProgress(int32_t combo) const;

    int32_t GetMaxCombo() const { return maxCombo_; }
    size_t GetMaxComboLevel() const { return kArraySize; }

private:
    static const size_t kArraySize = 2;

    using ThresholdPair = std::array<int32_t, kArraySize>;
    using ThresholdFloatPair = std::array<float, kArraySize>;

    int32_t maxCombo_ = 0;
    ThresholdPair thresholdsCount_{ 0,0 };// コンボ数の閾値

    ThresholdFloatPair thresholds_{ 0.3f,0.5f };
    const ThresholdPair kMaxComboLevels_{ 50, 100 };// 最低でもこのコンボ数で演出が入るようにする

};
