#include "NoteHoldEffect.h"

#include <System/Audio/AudioSystem.h>

#include <Application/Lane/Lane.h>
#include <System/Audio/SoundEngine.h>

using namespace Engine;


void NoteHoldEffect::Initialize()
{
    emitter_.Initialize("PopEffect");
    emitter2_.Initialize("PopEffect2");
    risingparticlesEmitter_.Initialize("hold_cube");

}

void NoteHoldEffect::Play(int32_t laneIndex)
{
    Vector3 lanePos = Lane::GetLaneEndPosition(laneIndex); // レーンの開始位置を取得

    Engine::SoundEngine::GetInstance()->PostEvent("OnHoldStart");

    emitter_.SetPosition(lanePos);
    emitter2_.SetPosition(lanePos);
    Vector3 offset = { 0.0f, 1.0f, 0.0f };
    risingparticlesEmitter_.SetPosition(lanePos + offset);

    emitter_.GenerateParticles();
    emitter2_.GenerateParticles(); // パーティクルを生成
    risingparticlesEmitter_.GenerateParticles();
}
