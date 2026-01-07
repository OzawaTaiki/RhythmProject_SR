#pragma once
#include <cstdint>
#include <map>
#include <Application/Note/Judge/JudgeType.h>

struct JudgementConfig
{
    float scoreValue;      // スコア
    float accuracyWeight;  // 精度重み
    float comboMultiplier; // コンボ倍率
    bool breaksCombo;      // コンボを途切れさせるか
};


class ScoreCalculator
{
public:

    ScoreCalculator() = default;
    ~ScoreCalculator() = default;

    void Initialize(int32_t totalNotes);

    void AddScore(JudgeType judgeType,int32_t currentCombo);

    int32_t GetDisplayScore()const;

private:



private:

    std::map<JudgeType, JudgementConfig> judgementConfigs = {
        { JudgeType::Perfect, {320.0f, 1.0f , 1.0f, false} },
        { JudgeType::Good,    {250.0f, 0.75f, 0.8f, false} },
        { JudgeType::Bad,     {150.0f, 0.4f , 0.5f, false} },
        { JudgeType::Miss,    {0.0f  , 0.0f , 0.0f, true } }
    };

    float noteScoreRatio_ = 0.7f;

    int32_t totalNotes_ = 0;
    float  rawScore_ = 0;
    float theoreticalMax_ = 0.0f;// 理論上の最大スコア

};