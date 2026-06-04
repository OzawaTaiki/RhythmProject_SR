#include "GameMusic.h"

#include <System/Audio/AudioSystem.h>

#include <Application/EventData/PauseActionData.h>
#include <Features/Event/EventManager.h>
#include <Math/MyLib.h>

using namespace Engine;

namespace
{
float gBitDepth = 0.8f;
float gSampleRateReduction = 0.3f;
constexpr const char* kBitCrusherEffectName = "bitcrusher";
}

GameMusic::GameMusic(const std::string& musicFilePath)
{
    soundInstance_ = AudioSystem::GetInstance()->Load(musicFilePath);
}

GameMusic::~GameMusic()
{
    if (voiceInstance_)
    {
        voiceInstance_.reset();
    }
}

void GameMusic::Initialize(float rewindTime)
{
    // コールバックの設定
    voiceCallBack_ = std::make_unique<VoiceCallBack>();
    voiceCallBack_->SetOnStreamEndCallback([this]()
                                           {
                                               MusicEnd();
                                               EventManager::GetInstance()->DispatchEvent(GameEvent("MusicEnded", nullptr));
                                           }); // 音楽が終了したときのコールバックを設定

    rewindTime_ = rewindTime;
    pausedAtTime_ = 0.0f;
    isMusicPlaying_ = false;
    // VST3 の初期化は AudioEffectManager::LoadEffectData() で済み
}

void GameMusic::Update(float deltaTime)
{
#ifdef _DEBUG
    //ImGui::Begin("GameMusic Debug");

    //ImGui::DragFloat("Bit Depth", &gBitDepth, 0.01f, 0.0f, 1.0f);
    //ImGui::DragFloat("Sample Rate Reduction", &gSampleRateReduction, 0.01f, 0.0f, 1.0f);

    //ImGui::End();
#endif

    UpdateDucking(deltaTime);
}

float GameMusic::GetElapsedTime() const
{
    if (voiceInstance_)
    {
        return voiceInstance_->GetElapsedTime();
    }
    return 0.0f; // voiceInstanceがない場合は0を返す
}

void GameMusic::Play(float volume)
{
    if (isMusicPlaying_)
        return;

    if (voiceInstance_)
        voiceInstance_.reset();

    GenerateVoiceWithBitCrusher(volume, 0.0f);
    voiceInstance_->Play();
    isMusicPlaying_ = true;

}

void GameMusic::Resume()
{
    if (voiceInstance_ && !isMusicPlaying_)
    {
        voiceInstance_->Play();
        isMusicPlaying_ = true;
    }
}

void GameMusic::ResumeWithRewind(float volume)
{
    if (voiceInstance_ && !isMusicPlaying_)
    {
        voiceInstance_.reset();

        // 巻き戻し再生の開始時間を計算
        float startTime = pausedAtTime_ - rewindTime_;
        startTime = (std::max)(startTime, 0.0f); // 負の値にならないようにする

        GenerateVoiceWithBitCrusher(volume, startTime);

        // 再生を開始
        voiceInstance_->Play();
        isMusicPlaying_ = true;
    }
}

void GameMusic::Pause()
{
    if (voiceInstance_ && isMusicPlaying_)
    {
        pausedAtTime_ = voiceInstance_->GetElapsedTime(); // 現在の再生時間を記録

        voiceInstance_->Pause();
        isMusicPlaying_ = false;
    }
}

void GameMusic::SetVolume(float volume)
{
    if (voiceInstance_)
    {
        voiceInstance_->SetVolume(volume);
    }
}

float GameMusic::GetDuration() const
{
    if (soundInstance_)
    {
        return soundInstance_->GetDuration();
    }

    return 0.0f; // soundInstanceがない場合は0を返す
}

std::shared_ptr<VoiceInstance> GameMusic::GetVoiceInstance()
{
    if (voiceInstance_)
        return voiceInstance_;

    return nullptr;
}

std::shared_ptr<SoundInstance> GameMusic::GetSoundInstance()
{
    if (soundInstance_)
        return soundInstance_;
    return nullptr;
}

void GameMusic::TriggerDucking(float targetVolume, float duration)
{
    duckingInfo_.isDucking = true;
    duckingInfo_.targetVolume = targetVolume;
    duckingInfo_.duckingDuration = duration;
    duckingInfo_.duckingDuration = 0.0f;
    duckingInfo_.duckingElapsed = 0.0f;
    //SetVolume(targetVolume); // ダッキングの目標音量に即座に設定
}

void GameMusic::SetBitCrush(float bitDepth, float sampleRateReduction)
{
    auto* paramMgr = AudioEffectManager::GetInstance()->GetParameterManager(kBitCrusherEffectName);
    if (paramMgr)
    {
        paramMgr->SetParameter(0, bitDepth);
        paramMgr->SetParameter(1, sampleRateReduction);
    }
}

void GameMusic::EnableBitCrush()
{
    effectChain_.EnableEffect(0);

    auto* paramMgr = AudioEffectManager::GetInstance()->GetParameterManager(kBitCrusherEffectName);
    if (paramMgr)
    {
        paramMgr->SetParameter(0, gBitDepth);
        paramMgr->SetParameter(1, gSampleRateReduction);
    }
}

void GameMusic::DisableBitCrush()
{
    effectChain_.DisableEffect(0);
}

void GameMusic::UpdateDucking(float deltaTime)
{
    if (!duckingInfo_.isDucking || !voiceInstance_)
        return;

    duckingInfo_.duckingElapsed += deltaTime;
    float t = std::min(duckingInfo_.duckingElapsed / duckingInfo_.duckingDuration, 1.0f);

    // ダッキングの目標音量から通常音量への線形補間
    float currentVolume = Lerp(duckingInfo_.targetVolume, DuckingInfo::kNormalVolume, t);
    SetVolume(currentVolume);

    auto* paramMgr = AudioEffectManager::GetInstance()->GetParameterManager(kBitCrusherEffectName);
    if (paramMgr)
    {
        paramMgr->SetParameter(0, gBitDepth);
        paramMgr->SetParameter(1, gSampleRateReduction);
    }

    if (t >= 1.0f)
    {
        duckingInfo_.isDucking = false; // ダッキング終了
    }

}

void GameMusic::GenerateVoiceWithBitCrusher(float volume, float startTime)
{
    auto* mgr = AudioEffectManager::GetInstance();
    auto* paramMgr = mgr->GetParameterManager(kBitCrusherEffectName);

    if (!paramMgr)
    {
        // エフェクトが未登録の場合はエフェクトなしで再生
        voiceInstance_ = soundInstance_->GenerateVoiceInstance(volume,
                                                               startTime,
                                                               false,
                                                               true,
                                                               voiceCallBack_.get(),
                                                               AudioSystem::GetInstance()->GetBGMSubmix());
        return;
    }

    effectChain_ = mgr->BuildEffectChain({ kBitCrusherEffectName });

    voiceInstance_ = soundInstance_->GenerateVoiceInstance(volume,
                                                           startTime,
                                                           false,
                                                           true,
                                                           voiceCallBack_.get(),
                                                           AudioSystem::GetInstance()->GetBGMSubmix(),
                                                           effectChain_.BuildChain());

    effectChain_.AttachToVoice(voiceInstance_->GetSourceVoice());
}
