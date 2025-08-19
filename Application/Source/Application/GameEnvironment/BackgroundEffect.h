#pragma once

#include <Application/GameEnvironment/GameEnvironment.h>

class BackgroundEffect
{
public:
    BackgroundEffect() = default;
    ~BackgroundEffect() = default;

    void PlaySpeakerEffect(uint32_t _laneIndex);

    void SetGameEnvironment(GameEnvironment* _env) { gameEnvironment_ = _env; }

private:

    GameEnvironment* gameEnvironment_ = nullptr; // 参照ポインタ

};