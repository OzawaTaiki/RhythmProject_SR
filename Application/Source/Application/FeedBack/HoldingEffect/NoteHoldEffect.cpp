#include "NoteHoldEffect.h"

#include <System/Audio/AudioSystem.h>

#include <Application/Lane/Lane.h>

using namespace Engine;


void NoteHoldEffect::Initialize()
{
    emitter_.Initialize("PopEffect");
    emitter2_.Initialize("PopEffect2");
    risingparticlesEmitter_.Initialize("hold_cube");

    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/SE/NoteHolding.wav");
}

void NoteHoldEffect::Play(int32_t laneIndex)
{
    Vector3 lanePos = Lane::GetLaneEndPosition(laneIndex); // レーンの開始位置を取得

    if (!voiceInstance_ || !voiceInstance_->IsPlaying())
    {
        voiceInstance_ = soundInstance_->Play(1.0f, false, true, nullptr, AudioSystem::GetInstance()->GetSESubmix());
    }

    emitter_.SetPosition(lanePos);
    emitter2_.SetPosition(lanePos);
    Vector3 offset = { 0.0f, 1.0f, 0.0f };
    risingparticlesEmitter_.SetPosition(lanePos + offset);

    emitter_.GenerateParticles();
    emitter2_.GenerateParticles(); // パーティクルを生成
    risingparticlesEmitter_.GenerateParticles();
}
