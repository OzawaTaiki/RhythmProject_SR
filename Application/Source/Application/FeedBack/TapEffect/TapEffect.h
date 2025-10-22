#pragma once

#include <cstdint>

// タップエフェクトクラス
class TapEffect
{
public:
    TapEffect() = default;
    ~TapEffect() = default;

    // 初期化
    void Initialize();

    // エフェクトを再生
    void Play(int32_t _laneIndex);
};