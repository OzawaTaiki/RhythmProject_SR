#include "RankCalculator.h"

std::map<Rank, float> RankCalculator::rankThresholds_ = {
    { Rank::S, 0.9f },
    { Rank::A, 0.75f },
    { Rank::B, 0.6f },
    { Rank::C, 0.0f }
};

Rank RankCalculator::CalculateRank(int32_t score, int32_t maxScore)
{
    float percentage = static_cast<float>(score) / static_cast<float>(maxScore);
    for (const auto& [rank, threshold] : rankThresholds_)
    {
        if (percentage >= threshold)
        {
            return rank;
        }
    }

    return Rank::C;
}

std::string RankCalculator::GetRankString(Rank rank)
{
    switch (rank)
    {
        case Rank::S:
            return "S";
        case Rank::A:
            return "A";
        case Rank::B:
            return "B";
        case Rank::C:
        default:
            return "C";
    }
}
