#include "BackgroundEffect.h"
#include <Application/GameBackground/SpeakerEffect.h>

using namespace Engine;

void BackgroundEffect::PlaySpeakerEffect(uint32_t laneIndex, float delayTime)
{
    if (!gameBackground_)
        return;

    // スピーカーオブジェクト取得
    ObjectModel* speaker = gameBackground_->GetSpeaker(laneIndex);
    if (!speaker)
        return;

    // スピーカーの座標を取得
    Vector3 speakerPos = speaker->GetWorldTransform()->GetWorldPosition();

    // スピーカーエフェクト再生
    SpeakerEffect effect;
    effect.PlaySpeakerEffect(speaker,delayTime);
}
