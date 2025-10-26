#pragma once

#include <cstdint>

/// <summary>
/// キーの状態を表す列挙型。
/// </summary>
enum class KeyState
{
    None,      // 何も押されていない
    trigger,   // 押された
    Released,  // 離された
    Hold,       // 押し続けている

    MAX // キー状態の最大値
};

/// <summary>
/// 入力データを表す構造体。
/// </summary>
struct InputDate
{
    KeyState state = KeyState::None; // キーの状態
    float elapsedTime = 0.0f; // 経過時間
    int32_t laneIndex = -1; // レーンのインデックス (0から始まる)
};
