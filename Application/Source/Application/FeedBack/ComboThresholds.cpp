#include "ComboThresholds.h"

const size_t ComboThresholds::kArraySize;

void ComboThresholds::Initialize(int32_t maxCombo)
{
    maxCombo_ = maxCombo;
    for (size_t i = 0; i < kArraySize; ++i)
    {
        // コンボ数の閾値を計算
        thresholdsCount_[i] = std::min(static_cast<int32_t>(maxCombo_ * thresholds_[i]), kMaxComboLevels_[i]);
    }
}

int32_t ComboThresholds::GetComboLevel(int32_t combo) const
{
    for (size_t i = 0; i < kArraySize; ++i)
    {
        if (combo < thresholdsCount_[i])
        {
            return static_cast<int32_t>(i);
        }
    }
    return static_cast<int32_t>(kArraySize);
}

float ComboThresholds::GetComboProgress(int32_t combo) const
{
    for (size_t i = 0; i < kArraySize; ++i)
    {
        if (combo < thresholdsCount_[i])
        {
            float lowerBound = (i == 0) ? 0.0f : static_cast<float>(thresholdsCount_[i - 1]);
            float upperBound = static_cast<float>(thresholdsCount_[i]);
            return (static_cast<float>(combo) - lowerBound) / (upperBound - lowerBound);
        }
    }
    return 1.0f;
}
