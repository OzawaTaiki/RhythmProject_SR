#include "SpectrumRing.h"

#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <Features/WaveformDisplay/WaveformAnalyzer.h>
#include <Math/MyLib.h>

namespace
{
float innerRadius = 1.0f;
float outerRadius = 3.0f;

size_t numRings = 5;
Vector4 clearColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

int32_t fftsize = 1024;
int32_t barCount = 48;
float minHz = 20.0f;
float maxHz = 10000.0f;

}

void SpectrumRing::Initialize(std::shared_ptr<SoundInstance> _musicInstance,size_t _numring)
{
    musicInstance_ = _musicInstance;
    numRings = _numring;
    audioSpectrum_ = AudioSpectrum();

    audioSpectrum_.SetAudioData(musicInstance_->GetAudioData());
    audioSpectrum_.SetSampleRate(musicInstance_->GetSampleRate());

    CreateRings();
    CreateTextureGenerators();
}

void SpectrumRing::Update(float _elapsedTime)
{
    if (!isInitTextures_)
    {
        auto spectrum = audioSpectrum_.GetSpectrumAtTime(0.0f);
        // 一度すべてのテクスチャを生成する
        // initではできないみたいなのでしかたない
        for (auto& generator : textureGenerators_)
        {
            generator->Generate(spectrum, 0.0f, barCount);
        }
        isInitTextures_ = true;
    }

    // テクスチャの生成
    // 毎フレーム1つだけ新しいデータで更新する
    // 古いテクスチャは使いまわす
    uint32_t index = cycleTextureIndices_.back();
    cycleTextureIndices_.pop_back();
    cycleTextureIndices_.push_front(index);

    auto spectrum = audioSpectrum_.GetSpectrumAtTime(_elapsedTime);
    float min, max;
    //WaveformAnalyzer::GetRawWaveformMaxMin(musicInstance_.get(), _elapsedTime, 5.0f, max, min);
    float scale =
        //(std::abs(max) + std::abs(min)) / 2.0f;
        WaveformAnalyzer::GetRMSAtTime(musicInstance_.get(),_elapsedTime);
    textureGenerators_[index]->Generate(spectrum, scale , barCount);
    uint32_t nextIndex = cycleTextureIndices_.back();
    textureGenerators_[nextIndex]->ReserveClear();


    if (beatManager_)
    {
        if(beatManager_->IsBeatTriggered())
        {
            for (size_t i = 1; i < rings_.size() - 1; ++i)
            {
                rings_[i]->euler_.z = rings_[i - 1]->euler_.z;
            }
        }
    }

     //TODO : リングの回転
    const float rotationSpeedPerSec = std::numbers::pi_v<float>;
    //rings_[0]->euler_.z = rotationSpeedPerSec * _elapsedTime;

    for (auto& ring : rings_)
    {
        ring->Update();
    }
}

void SpectrumRing::Draw(Camera* camera)
{
    for (size_t i = 0; i < 1; ++i)
    {
        uint32_t handleIndex = cycleTextureIndices_[i];
        rings_[i]->Draw(camera, textureHandles_[handleIndex], Vector4(1, 1, 1, 0.1f));
    }
}

void SpectrumRing::CreateRings()
{
    float outerRadiusStep = (outerRadius - innerRadius) / static_cast<float>(numRings);

    for (size_t i = 0; i < numRings; ++i)
    {
        // 大きい順で生成していく
        Ring priRing(innerRadius, outerRadius - outerRadiusStep * static_cast<float>(i));
        priRing.SetDivide(256); // 大きめにしないとテクスチャ崩れる

        std::string name = "spectrumRing_" + std::to_string(i);
        auto model = std::make_unique<ObjectModel>(name);
        model->Initialize(priRing.Generate(name));
        model->GetMaterial()->SetEnableLighting(false);
        rings_.emplace_back(std::move(model));
    }
}

void SpectrumRing::CreateTextureGenerators()
{
    textureGenerators_.resize(numRings);

    for (auto& generator : textureGenerators_)
    {
        generator = std::make_unique<SpectrumTextureGenerator>();
        generator->Initialize(clearColor);
        generator->MakeLogRanges(
            fftsize / 2,
            barCount,
            minHz,
            maxHz,
            musicInstance_->GetSampleRate(),
            fftsize
        );

        textureHandles_.push_back(generator->GetTextureHandle());
    }

    for (uint32_t i = 0; i < numRings; ++i)
    {
        uint32_t num = static_cast<uint32_t>(numRings - 1) - i;
        cycleTextureIndices_.push_back(i);// 4 3 2 1 0
    }

}
