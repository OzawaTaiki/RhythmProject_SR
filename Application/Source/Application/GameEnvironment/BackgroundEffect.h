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
    /// <param name="laneIndex">レーンインデックス</param>
    void PlaySpeakerEffect(uint32_t laneIndex,float delayTime);

    /// <summary>
    /// 管理する GameEnvironment を設定する。
    /// </summary>
    /// <param name="env">GameEnvironment のポインタ</param>
    void SetGameBackground(GameBackground* bg) { gameBackground_ = bg; }

private:
    GameBackground* gameBackground_ = nullptr; // 参照ポインタ
};