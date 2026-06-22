#include "GameMusic.h"

#include <System/Audio/AudioSystem.h>
#include <System/Audio/BitcrusherProcessor.h>

#include <Application/EventData/PauseActionData.h>
#include <Features/Event/EventManager.h>
#include <Math/MyLib.h>

#ifdef _DEBUG
#include <imgui.h>
#endif

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

    // スペクトラム比較用の解析器を初期化
    constexpr size_t kFFTSize = 32768;
    audioSpectrumPre_  = std::make_unique<Engine::AudioSpectrum>(kFFTSize);
    audioSpectrumPost_ = std::make_unique<Engine::AudioSpectrum>(kFFTSize);

    if (soundInstance_)
    {
        float sampleRate = soundInstance_->GetSampleRate();

        audioSpectrumPre_->SetAudioData(soundInstance_->GetAudioData());
        audioSpectrumPre_->SetSampleRate(sampleRate);
        audioSpectrumPre_->SetUseGPU(false);

        audioSpectrumPost_->SetSampleRate(sampleRate);
        audioSpectrumPost_->SetUseGPU(false);
        // post は Update() 内で現在時刻の窓だけ処理するため、ここでは設定不要
    }
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
    if (ImGui::Begin("Bitcrusher Spectrum Comparison"))
    {
        bool paramsChanged = false;
        paramsChanged |= ImGui::SliderFloat("Bit Depth",            &bitDepth_,            0.0f, 1.0f);
        paramsChanged |= ImGui::SliderFloat("Sample Rate Reduction",&sampleRateReduction_, 0.0f, 1.0f);
        if (paramsChanged)
            SetBitCrush(bitDepth_, sampleRateReduction_);

        // 現在時刻のスペクトラムを取得して描画
        if (voiceInstance_ && audioSpectrumPre_ && audioSpectrumPost_ && soundInstance_)
        {
            float t  = voiceInstance_->GetElapsedTime();
            float sr = soundInstance_->GetSampleRate();

            // 現在時刻付近の窓(32768サンプル)だけ取り出してビットクラッシャーを適用
            constexpr size_t kWin = 32768;
            auto rawData = soundInstance_->GetAudioData();
            size_t center = static_cast<size_t>(t * sr);
            size_t start  = (center >= kWin / 2) ? center - kWin / 2 : 0;
            start = (rawData.size() > kWin) ? std::min(start, rawData.size() - kWin) : 0;
            size_t count  = std::min(kWin, rawData.size() - start);
            std::vector<float> window(rawData.begin() + start, rawData.begin() + start + count);
            window.resize(kWin, 0.0f);

            auto crushed = BitcrusherProcessor::Process(window, bitDepth_, sampleRateReduction_);
            audioSpectrumPost_->SetAudioData(crushed);

            auto pre  = audioSpectrumPre_->GetSpectrumAtTime(t);
            float windowCenter = static_cast<float>(kWin / 2) / sr;
            auto post = audioSpectrumPost_->GetSpectrumAtTime(windowCenter);

            // 表示用に対数スケールで正規化（0dB = -80dB..0dB → 0..1）
            auto toNorm = [](float amp) -> float {
                constexpr float kMinDb = -80.0f;
                float db = (amp > 1e-10f) ? 20.0f * std::log10(amp) : kMinDb;
                return std::clamp((db - kMinDb) / (-kMinDb), 0.0f, 1.0f);
            };

            constexpr int kBars = 64;
            const int preSize  = static_cast<int>(pre.size());
            const int postSize = static_cast<int>(post.size());

            // Pre（白）
            ImGui::TextUnformatted("Pre-effect (white)");
            for (int i = 0; i < kBars; ++i)
            {
                int idx = i * preSize / kBars;
                float v = (idx < preSize) ? toNorm(pre[idx]) : 0.0f;
                char label[8];
                snprintf(label, sizeof(label), "##p%d", i);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1, 1, 1, 0.9f));
                ImGui::ProgressBar(v, ImVec2(8.0f, 80.0f), label);
                ImGui::PopStyleColor();
                if (i < kBars - 1) ImGui::SameLine(0.0f, 1.0f);
            }

            // Post（橙）
            ImGui::Spacing();
            ImGui::TextUnformatted("Post-effect / bitcrushed (orange)");
            for (int i = 0; i < kBars; ++i)
            {
                int idx = i * postSize / kBars;
                float v = (idx < postSize) ? toNorm(post[idx]) : 0.0f;
                char label[8];
                snprintf(label, sizeof(label), "##q%d", i);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1, 0.55f, 0.1f, 0.9f));
                ImGui::ProgressBar(v, ImVec2(8.0f, 80.0f), label);
                ImGui::PopStyleColor();
                if (i < kBars - 1) ImGui::SameLine(0.0f, 1.0f);
            }
        }
    }
    ImGui::End();
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
    bitDepth_ = bitDepth;
    sampleRateReduction_ = sampleRateReduction;

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

    bitDepth_ = gBitDepth;
    sampleRateReduction_ = gSampleRateReduction;

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
