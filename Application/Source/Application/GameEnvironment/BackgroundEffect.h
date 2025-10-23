#pragma once

#include <Application/GameEnvironment/GameEnvironment.h>

/// <summary>
/// 背景エフェクト。
/// </summary>
class BackgroundEffect
{
public:
    BackgroundEffect() = default;
    ~BackgroundEffect() = default;

    /// <summary>
    /// 指定レーンのスピーカーエフェクトを再生する。
    /// </summary>
    /// <param name="_laneIndex">レーンインデックス</param>
    void PlaySpeakerEffect(uint32_t _laneIndex);

    /// <summary>
    /// 管理する GameEnvironment を設定する。
    /// </summary>
    /// <param name="_env">GameEnvironment のポインタ</param>
    void SetGameEnvironment(GameEnvironment* _env) { gameEnvironment_ = _env; }

private:
    GameEnvironment* gameEnvironment_ = nullptr; // 参照ポインタ
};