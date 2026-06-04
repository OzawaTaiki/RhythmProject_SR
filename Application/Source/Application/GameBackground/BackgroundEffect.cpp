#include "BackgroundEffect.h"
#include <Application/GameBackground/SpeakerEffect.h>

using namespace Engine;

BackgroundEffect::BackgroundEffect()
{
    // エミッタのJSONロードはここで1回だけ行い、以降は使い回す
    speakerEffect_ = std::make_unique<SpeakerEffect>();
}

BackgroundEffect::~BackgroundEffect() = default;

void BackgroundEffect::PlaySpeakerEffect(uint32_t laneIndex, float delayTime)
{
    if (!gameBackground_)
        return;

    // スピーカーオブジェクト取得
    ObjectModel* speaker = gameBackground_->GetSpeaker(laneIndex);
    if (!speaker)
        return;

    // スピーカーエフェクト再生（使い回しのインスタンス）
    speakerEffect_->PlaySpeakerEffect(speaker, delayTime);
}
