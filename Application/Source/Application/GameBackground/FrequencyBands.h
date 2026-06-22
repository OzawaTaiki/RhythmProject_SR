#pragma once

struct FrequencyBands
{
    static constexpr float kSubBass    = 60.0f;    // 0    - 60   Hz (重低音)
    static constexpr float kBass       = 250.0f;   // 60   - 250  Hz (ベース)
    static constexpr float kLowMid     = 500.0f;   // 250  - 500  Hz (低域ミッド)
    static constexpr float kMid        = 2000.0f;  // 500  - 2000 Hz (中域)
    static constexpr float kHighMid    = 6000.0f;  // 2000 - 6000 Hz (高域ミッド)
    static constexpr float kHigh       = 22000.0f; // 6000 - 22000Hz (高音)
};