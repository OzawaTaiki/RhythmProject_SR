#pragma once
#include <array>


/// <summary>
/// コンボ数に応じた演出レベルの閾値を管理するクラス。
/// Initialize() で譜面のmaxComboを渡すと各レベルの境界コンボ数を自動計算する。
/// GetComboLevel() で現在のレベル(0始まり)、GetComboProgress() でレベル内進捗(0-1)を取得する。
/// </summary>
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


    float GetMissEffectThreshold(int32_t combo) const;
private:
    static const size_t kArraySize = 2;

    using ThresholdPair = std::array<int32_t, kArraySize>;
    using ThresholdFloatPair = std::array<float, kArraySize>;

    int32_t prevLevel_ = 0;

    int32_t maxCombo_ = 0;
    ThresholdPair thresholdsCount_{ 0,0 };// コンボ数の閾値

    ThresholdFloatPair thresholds_{ 0.3f,0.5f };
    const ThresholdPair kMaxComboLevels_{ 50, 100 };// 最低でもこのコンボ数で演出が入るようにする

};
