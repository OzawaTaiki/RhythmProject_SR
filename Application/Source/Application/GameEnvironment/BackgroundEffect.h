#pragma once

#include <Application/GameEnvironment/GameEnvironment.h>

// 背景エフェクト
class BackgroundEffect
{
public:
    BackgroundEffect() = default;
    ~BackgroundEffect() = default;

    // スピーカーエフェクト再生
    void PlaySpeakerEffect(uint32_t _laneIndex);
    // 背景オブジェクトクラスをセット
    void SetGameEnvironment(GameEnvironment* _env) { gameEnvironment_ = _env; }

private:

    GameEnvironment* gameEnvironment_ = nullptr; // 参照ポインタ

};