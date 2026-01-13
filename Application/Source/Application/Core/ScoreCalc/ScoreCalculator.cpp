#include "ScoreCalculator.h"
#include <Debug/Debug.h>

namespace
{
const int32_t kBaseScore = 100000; // 100,000点満点
const int32_t kMaxComboForBonus = 100;
const float kMaxComboMultiplier = 1.5f;// コンボ倍率の最大値
}

void ScoreCalculator::Initialize(int32_t totalNotes)
{
    totalNotes_ = totalNotes;
    rawScore_ = 0.0f;

    theoreticalMax_ = 0.0f;
    Debug::Log(std::format("=========================\nmaxComboFotBonus_ : {}\n", kMaxComboForBonus));
    Debug::Log(std::format("Perfect Score Value: {}\n", judgementConfigs[JudgeType::Perfect].scoreValue));
    Debug::Log(std::format("Perfect Accuracy Weight: {}\n", judgementConfigs[JudgeType::Perfect].accuracyWeight));
    for (int32_t i = 0; i < totalNotes; i++)
    {
        // i番目のノーツでのコンボ倍率
        float comboRatio = std::min((float)i / kMaxComboForBonus, 1.0f);
        float multiplier = 1.0f + comboRatio * (kMaxComboMultiplier - 1.0f);

        theoreticalMax_ += judgementConfigs[JudgeType::Perfect].scoreValue * multiplier;
        if (i == 0 || i == totalNotes)
        {
            Debug::Log(std::format("note{}: comboRatio = {}, multiplier = {},score = {}\n", i, comboRatio, multiplier,
                                   judgementConfigs[JudgeType::Perfect].scoreValue * multiplier));
        }
    }
    Debug::Log(std::format("Theoretical Max Score: {}\n=========================\n", theoreticalMax_));
}

void ScoreCalculator::AddScore(JudgeType judgeType, int32_t currentCombo)
{
    judgeType=JudgeType::Perfect;
    auto& config = judgementConfigs[judgeType];

    if (config.breaksCombo)
    {
        return;
    }

    // 現在のコンボボーナス倍率を計算
    float comboRatio = std::min(static_cast<float>(currentCombo) / static_cast<float>(kMaxComboForBonus), 1.0f);
    float comboMultiplier = 1.0f + comboRatio * (kMaxComboMultiplier - 1.0f);

    // スコア計算
    float noteRawScore = config.scoreValue * comboMultiplier * config.accuracyWeight;

    rawScore_ += noteRawScore;

    if (currentCombo == 0 || currentCombo == totalNotes_)
    {
        Debug::Log(std::format("=========================\nAdded Score: Combo = {},ratio = {}, mult ={},score ={}\n=========================\n",
                               currentCombo, comboRatio, comboMultiplier, noteRawScore));
    }
}

int32_t ScoreCalculator::GetDisplayScore() const
{
    Debug::Log(std::format("=========================\nRaw Score: {}, Theoretical Max: {}, Display Score: {}\n=========================\n", 
                           rawScore_, theoreticalMax_, static_cast<int32_t>((rawScore_ / theoreticalMax_) * kBaseScore)));
    return static_cast<int32_t>((rawScore_ / theoreticalMax_) * kBaseScore);
}

void ScoreCalculator::ScoreReset()
{
    rawScore_ = 0.0f;
}
