#pragma once

#include <cstdint>

class TapEffect
{
public:
    TapEffect() = default;
    ~TapEffect() = default;

    void Initialize();

    /// <summary>
    /// エフェクトを再生
    /// </summary>
    void Play(int32_t _laneIndex);

private:


};