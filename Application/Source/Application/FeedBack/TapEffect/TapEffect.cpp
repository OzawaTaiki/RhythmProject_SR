#include "TapEffect.h"

#include <Application/Lane/Lane.h>
#include <Application/Effects/TapEffects/TriggerEffects.h>

void TapEffect::Initialize()
{
    TriggerEffects::Initialize();
}

void TapEffect::Play(int32_t laneIndex)
{
    Vector3  lanePos = Lane::GetLaneEndPosition(laneIndex); // レーンの開始位置を取得

    // 中心の円を発生させる
    TriggerEffects::EmitCenterCircles(lanePos);

}
