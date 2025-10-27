#include "NoteHoldEffect.h"

#include <System/Audio/AudioSystem.h>

#include <Application/Lane/Lane.h>

void NoteHoldEffect::Initialize()
{
    emitter_.Initialize("PopEffect");
    emitter2_.Initialize("PopEffect2");
    risingparticlesEmitter_.Initialize("newTap_NewEmitter");

    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/SE/NoteHolding.wav");
}

void NoteHoldEffect::Play(int32_t _laneIndex)
{
    Vector3 lanePos = Lane::GetLaneEndPosition(_laneIndex); // レーンの開始位置を取得

    if (!voiceInstance_ || !voiceInstance_->IsPlaying())
    {
        voiceInstance_ = soundInstance_->Play(1.0f);
    }

    emitter_.SetPosition(lanePos);
    emitter2_.SetPosition(lanePos);
    risingparticlesEmitter_.SetPosition(lanePos);

    emitter_.GenerateParticles();
    emitter2_.GenerateParticles(); // パーティクルを生成
    risingparticlesEmitter_.GenerateParticles();

}
