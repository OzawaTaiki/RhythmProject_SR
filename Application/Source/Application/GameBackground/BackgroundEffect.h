#pragma once

#include <Application/GameBackground/GameEnvironment.h>

#include <memory>

class SpeakerEffect; // 前方宣言

/// <summary>
/// 背景エフェクト。
/// </summary>
class BackgroundEffect
{
public:
    BackgroundEffect();
    ~BackgroundEffect();

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
    // スピーカーエフェクトは1つを使い回す（エミッタJSONの再ロードを避ける）
    std::unique_ptr<SpeakerEffect> speakerEffect_;
};