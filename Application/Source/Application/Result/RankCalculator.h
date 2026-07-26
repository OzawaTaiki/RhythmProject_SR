#pragma once

#include <cstdint>
#include <map>
#include <string>

/// <summary>
/// Rankを表す列挙型。
/// </summary>
enum class Rank
{
    S, A, B, C
};

/// <summary>
/// RankCalculatorを表すクラス。
/// </summary>
class RankCalculator
{
public:

    RankCalculator() = default;
    ~RankCalculator() = default;

    static Rank CalculateRank(int32_t score, int32_t maxScore);
    static std::string GetRankString(Rank rank);

private:

    static std::map<Rank, float> rankThresholds_;

};
