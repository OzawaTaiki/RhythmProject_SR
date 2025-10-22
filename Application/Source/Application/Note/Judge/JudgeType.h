#pragma once

#include <string>

// 判定タイプ
enum class JudgeType
{
    None = 0,

    Perfect,
    Good,
    Bad,
    Miss,

    MAX
};
namespace Judge
{
// 判定タイプを文字列に変換
std::string ToString(JudgeType _type);
}
