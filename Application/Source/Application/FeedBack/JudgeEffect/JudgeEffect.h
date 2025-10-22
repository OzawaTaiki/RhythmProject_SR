#pragma once

#include <cstdint>

// 判定時にエフェクト
class JudgeEffect
{
public:
    JudgeEffect() = default;
    ~JudgeEffect() = default;

    // 初期化
    void Initialize();

    // エフェクトを再生
    void Play(int32_t _laneIndex);
};