#include "SpectrumRing.h"

#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <Features/WaveformDisplay/WaveformAnalyzer.h>
#include <Math/MyLib.h>

#ifdef _DEBUG
#include <Debug/ImGuiDebugManager.h>
#include <imgui.h>
#endif

using namespace Engine;

namespace
{
float innerRadius = 1.0f;
float outerRadius = 3.0f;

size_t numRings = 5;
Vector4 clearColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

int32_t fftsize = 32768;
int32_t barCount = 48;
float minHz = 20.0f;
float maxHz = 10000.0f;

}

void SpectrumRing::Initialize(std::shared_ptr<SoundInstance> musicInstance, size_t numring)
{
    musicInstance_ = musicInstance;
    numRings = numring;
    audioSpectrum_ = std::make_unique<AudioSpectrum>(fftsize);

    audioSpectrum_->SetAudioData(musicInstance_->GetAudioData());
    audioSpectrum_->SetSampleRate(musicInstance_->GetSampleRate());

    CreateRings();
    CreateTextureGenerators();
}

void SpectrumRing::Update(float elapsedTime)
{
#ifdef _DEBUG
    if (ImGuiDebugManager::GetInstance()->Begin("SpectrumRing"))
    {
        static int n = 15; // 2^15 = 32768
        ImGui::SliderInt("N (FFT = 2^N)", &n, 10, 16);
        ImGui::Text("= %d", 1 << n);
        if (ImGui::Button("Apply"))
        {
            fftsize = 1 << n;
            audioSpectrum_->SetFFTSize(fftsize);
            for (auto& gen : textureGenerators_)
                gen->MakeLogRanges(musicInstance_->GetSampleRate(), minHz, maxHz, barCount, fftsize / 2, fftsize);
            isInitTextures_ = false;
        }
        ImGui::Text("Current: %d", fftsize);
        ImGui::End();
    }
    if (ImGuiDebugManager::GetInstance()->Begin("Fourier Transform"))
    {
        // 毎フレームFourier Transformを出すために空で
        ImGui::End();
    }

#endif

    if (!isInitTextures_)
    {
        auto spectrum = audioSpectrum_->GetSpectrumAtTime(0.0f);
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

    auto spectrum = audioSpectrum_->GetSpectrumAtTime(elapsedTime);
    //float min, max;
    //WaveformAnalyzer::GetRawWaveformMaxMin(musicInstance_.get(), _elapsedTime, 5.0f, max, min);
    float scale =
        //(std::abs(max) + std::abs(min)) / 2.0f;
        WaveformAnalyzer::GetRMSAtTime(musicInstance_.get(), elapsedTime);
    textureGenerators_[index]->Generate(spectrum, scale, barCount);
    uint32_t nextIndex = cycleTextureIndices_.back();
    textureGenerators_[nextIndex]->ReserveClear();

    // 棒の位置は変わらない rot速度は一定間隔
    // 1箇所当たり三本
    // 1は最新 2，3は過去
    // 過去は徐々に短くなる
    // 過去は毎フレームは更新されない
    // じゃあどのタイミングで更新するか
    // TODO : わからん
    //

     //TODO : リングの回転
    const float rotationSpeedPerFrame = std::numbers::pi_v<float> *2.0f / static_cast<float>(barCount * 3);
    rings_[0]->euler_.z += rotationSpeedPerFrame;
    for (size_t i = 1; i < rings_.size(); ++i)
    {
        rings_[i]->euler_.z = rings_[i - 1]->euler_.z + (rotationSpeedPerFrame * static_cast<float>(barCount * 3 / numRings));
    }

    for (auto& ring : rings_)
    {
        ring->Update();
    }
}

void SpectrumRing::Draw(Camera* camera)
{
    for (size_t i = 0; i < numRings; ++i)
    {
        uint32_t handleIndex = cycleTextureIndices_[i];
        rings_[i]->Draw(camera, textureHandles_[handleIndex], Vector4(1, 1, 1, 0.3f));
    }    rings_[numRings]->Draw(camera);
}

void SpectrumRing::SetMusicInstance(std::shared_ptr<Engine::SoundInstance> musicInstance)
{
    if (!musicInstance)
        return;

    musicInstance_ = musicInstance;
    audioSpectrum_->SetAudioData(musicInstance_->GetAudioData());
    audioSpectrum_->SetSampleRate(musicInstance_->GetSampleRate());
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
        model->GetMaterial()->GetUVTransform().SetUScale(3.0f);
        rings_.emplace_back(std::move(model));
    }

    //  中のリング
    Ring innerRing(0.0f, innerRadius);
    innerRing.SetDivide(256);
    auto innerModel = std::make_unique<ObjectModel>("spectrumInnerRing");
    innerModel->Initialize(innerRing.Generate("spectrumInnerRing"));
    innerModel->GetMaterial()->SetEnableLighting(false);
    rings_.emplace_back(std::move(innerModel));
}

void SpectrumRing::CreateTextureGenerators()
{
    textureGenerators_.resize(numRings);

    for (auto& generator : textureGenerators_)
    {
        generator = std::make_unique<SpectrumTextureGenerator>();
        generator->Initialize(clearColor);
        generator->MakeLogRanges(
            musicInstance_->GetSampleRate(),
            minHz,
            maxHz,
            barCount,
            fftsize / 2,
            fftsize
        );

        textureHandles_.push_back(generator->GetTextureHandle());
    }

    for (uint32_t i = 0; i < numRings; ++i)
    {
        cycleTextureIndices_.push_back(i);// 4 3 2 1 0
    }

}