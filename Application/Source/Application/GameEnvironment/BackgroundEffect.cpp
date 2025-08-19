#include "BackgroundEffect.h"
#include <Application/GameEnvironment/SpeakerEffect.h>

void BackgroundEffect::PlaySpeakerEffect(uint32_t _laneIndex)
{
    if (gameEnvironment_)
    {
        ObjectModel* speaker = gameEnvironment_->GetSpeaker(_laneIndex);
        if (speaker)
        {
            Vector3 speakerPos = speaker->GetWorldTransform()->GetWorldPosition();

            SpeakerEffect effect;
            effect.PlaySpeakerEffect(speaker);
        }
    }
}
