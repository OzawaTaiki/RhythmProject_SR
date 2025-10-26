#include "JudgeEffect.h"

#include <Application/Lane/Lane.h>
#include <Application/Effects/TapEffects/TriggerEffects.h>

void JudgeEffect::Initialize()
{
    TriggerEffects::Initialize();
}

void JudgeEffect::Play(int32_t _laneIndex)
{
    Vector3  lanePos = Lane::GetLaneEndPosition(_laneIndex); // レーンの開始位置を取得

    // 周囲のパーティクルを発生させる
    TriggerEffects::EmitSurroundingParticles(lanePos);
}
